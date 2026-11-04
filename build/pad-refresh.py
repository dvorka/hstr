#!/usr/bin/env python3
#
#  Copyright (C) 2014-2026 Martin Dvorak <martin.dvorak@mindforger.com>
#
#  Licensed under the Apache License, Version 2.0 (the "License");
#  you may not use this file except in compliance with the License.
#  You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
#  Unless required by applicable law or agreed to in writing, software
#  distributed under the License is distributed on an "AS IS" BASIS,
#  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#  See the License for the specific language governing permissions and
#  limitations under the License.

# refresh pad.xml release fields from the single sources of truth:
#
# - Program_Version                          from VERSION_STRING in src/hstr.c
# - Program_Release_Month/Day/Year           from VERSION_STRING in src/hstr.c
# - Program_Change_Info                      from the latest Changelog entry
# - Program_Release_Status                   from the version bump (vs. the
#                                            previous Changelog entry)
# - File_Size_Bytes/File_Size_K/File_Size_MB from the newest release tarball
#                                            built by tarball-build.sh, when
#                                            one is found
#
# the script uses Python stdlib only and never shells out; pad.xml is edited
# in place with targeted text replacements to preserve its formatting

import datetime
import pathlib
import re
import sys
from xml.sax import saxutils

ROOT = pathlib.Path(__file__).resolve().parent.parent
PAD_FILE = ROOT / "pad.xml"
HSTR_SOURCE_FILE = ROOT / "src" / "hstr.c"
CHANGELOG_FILE = ROOT / "Changelog"
# release directory used by build/tarball/tarball-build.sh (~/p/hstr/release
# for a ~/p/hstr/github/hstr checkout)
RELEASE_DIR = ROOT.parent.parent / "release"

RELEASE_STATUS = {
    "major": "Major Update",
    "minor": "Minor Update",
    "patch": "Minor Bug Fixes",
}

# PAD Program_Change_Info should stay concise; cap the generated summary
MAX_CHANGE_INFO_CHARS = 500

VERSION_RE = re.compile(
    r'hstr version \\"(\d+\.\d+\.\d+)\\" \((\d{4}-\d{2}-\d{2})T'
)
CHANGELOG_HEADER_RE = re.compile(r"^\d{4}-\d{2}-\d{2}\s")
RELEASED_RE = re.compile(r"^\*\s+Released\s+v(\d+\.\d+\.\d+)")


def fail(message: str) -> None:
    sys.exit(f"ERROR: {message}")


def read_text(path: pathlib.Path) -> str:
    """Read ``path`` keeping its original line endings (pad.xml uses CRLF)."""
    try:
        with path.open(encoding="utf-8", newline="") as file:
            return file.read()
    except OSError as error:
        fail(f"unable to read {path}: {error}")


def write_text(path: pathlib.Path, text: str) -> None:
    """Write ``path`` without translating line endings."""
    try:
        with path.open("w", encoding="utf-8", newline="") as file:
            file.write(text)
    except OSError as error:
        fail(f"unable to write {path}: {error}")


def read_version() -> tuple:
    """Return (version, release date) from VERSION_STRING in src/hstr.c."""
    match = VERSION_RE.search(read_text(HSTR_SOURCE_FILE))
    if not match:
        fail(f"could not parse VERSION_STRING from {HSTR_SOURCE_FILE}")
    try:
        date = datetime.date.fromisoformat(match.group(2))
    except ValueError:
        fail(f"invalid release date '{match.group(2)}' in {HSTR_SOURCE_FILE}")
    return match.group(1), date


def strip_markdown(line: str) -> str:
    """Reduce a Changelog item to plain, single-line text."""
    line = re.sub(r"\[([^\]]+)\]\([^)]+\)", r"\1", line)
    line = line.replace("`", "").replace("**", "").replace("*", "")
    return re.sub(r"\s+", " ", line).strip()


def collect_items(block: list) -> list:
    """Return the ``- `` prefixed items of a Changelog release entry.

    An item may wrap across following indented continuation lines that are
    neither blank nor the start of a new item.
    """
    items = []
    current = []

    def flush():
        if current:
            items.append(strip_markdown(" ".join(current)))
            current.clear()

    for line in block:
        stripped = line.strip()
        if stripped.startswith("- "):
            flush()
            current.append(stripped[2:])
        elif current and stripped and not stripped.startswith("* "):
            current.append(stripped)
        else:
            flush()
    flush()
    return items


def changelog_versions() -> list:
    """Return [(version, [items]), ...] for every ``* Released vX.Y.Z`` entry.

    HSTR's ``Changelog`` uses the GNU ChangeLog format: dated blocks with
    a ``* Released vX.Y.Z - <kind> release:`` bullet followed by ``- ``
    prefixed items. Entries are returned newest-first, as they appear
    in the file.
    """
    blocks = []
    current = None
    for line in read_text(CHANGELOG_FILE).splitlines():
        if CHANGELOG_HEADER_RE.match(line):
            current = []
            blocks.append(current)
        elif current is not None:
            current.append(line)

    versions = []
    for block in blocks:
        for index, line in enumerate(block):
            match = RELEASED_RE.match(line.strip())
            if match:
                versions.append((match.group(1), collect_items(block[index + 1:])))
                break
    return versions


def release_status(version: str, previous_version) -> str:
    """Classify ``version`` vs. ``previous_version`` as major/minor/patch."""
    major, minor, patch = (int(p) for p in version.split("."))
    if previous_version is None:
        release_type = "major" if not (minor or patch) else "minor"
    else:
        p_major, p_minor, _ = (int(p) for p in previous_version.split("."))
        if major != p_major:
            release_type = "major"
        elif minor != p_minor:
            release_type = "minor"
        else:
            release_type = "patch"
    return RELEASE_STATUS[release_type]


def change_info(version: str, items: list) -> str:
    """Join Changelog items into a summary which fits the PAD length limit.

    Only whole items are used so that the summary is never cut mid-sentence;
    if not all of them fit, the summary ends with an ellipsis.
    """
    summary = ""
    for item in items:
        candidate = f"{summary}; {item}" if summary else item
        if len(candidate) + len(".") > MAX_CHANGE_INFO_CHARS:
            return f"{summary}..." if summary else f"HSTR {version} release."
        summary = candidate
    return f"{summary}." if summary else f"HSTR {version} release."


def tarball_size_bytes(version: str):
    """Return the size in bytes of the newest release tarball for ``version``.

    Tarballs are built by ``build/tarball/tarball-build.sh`` into
    ``<release dir>/release-<timestamp>/hstr-<version>-tarball.tgz``.
    Returns None when nothing has been built locally - pad.xml's file size
    fields are then left unchanged.
    """
    if not RELEASE_DIR.is_dir():
        return None
    tarballs = sorted(
        path
        for path in RELEASE_DIR.glob(f"release-*/hstr-{version}-tarball.tgz")
        if path.is_file()
    )
    # release directory names embed a sortable timestamp, newest is the last
    return tarballs[-1].stat().st_size if tarballs else None


def set_tag(xml: str, tag: str, value: str) -> str:
    """Replace the text content of ``<tag>`` (open/close or self-closing form)."""
    escaped = saxutils.escape(value)
    paired = re.compile(rf"<{tag}>.*?</{tag}>", re.DOTALL)
    if paired.search(xml):
        return paired.sub(lambda _: f"<{tag}>{escaped}</{tag}>", xml, count=1)
    selfclosing = re.compile(rf"<{tag}\s*/>")
    if selfclosing.search(xml):
        return selfclosing.sub(lambda _: f"<{tag}>{escaped}</{tag}>", xml, count=1)
    fail(f"tag <{tag}> not found in {PAD_FILE}")


def main() -> None:
    version, release_date = read_version()

    versions = changelog_versions()
    if not versions or versions[0][0] != version:
        fail(
            f"no Changelog entry '* Released v{version}' found as the "
            f"latest entry in {CHANGELOG_FILE}"
        )
    items = versions[0][1]
    previous_version = versions[1][0] if len(versions) > 1 else None

    status = release_status(version, previous_version)
    summary = change_info(version, items)

    xml = read_text(PAD_FILE)
    xml = set_tag(xml, "Program_Version", version)
    xml = set_tag(xml, "Program_Release_Month", f"{release_date.month:02d}")
    xml = set_tag(xml, "Program_Release_Day", f"{release_date.day:02d}")
    xml = set_tag(xml, "Program_Release_Year", f"{release_date.year}")
    xml = set_tag(xml, "Program_Release_Status", status)
    xml = set_tag(xml, "Program_Change_Info", summary)

    size = tarball_size_bytes(version)
    if size is not None:
        xml = set_tag(xml, "File_Size_Bytes", str(size))
        xml = set_tag(xml, "File_Size_K", f"{size / 1024:.2f}")
        xml = set_tag(xml, "File_Size_MB", f"{size / 1024 / 1024:.2f}")

    write_text(PAD_FILE, xml)

    print(f"pad.xml refreshed for HSTR {version} ({release_date.isoformat()})")
    print(f"  release status: {status}")
    print(f"  change info:    {summary[:80]}...")
    if size is not None:
        print(f"  tarball size:   {size} bytes ({size / 1024 / 1024:.2f} MB)")
    else:
        print(f"  tarball size:   unchanged (no hstr-{version}-tarball.tgz in {RELEASE_DIR})")
    print("DONE")


if __name__ == "__main__":
    main()
