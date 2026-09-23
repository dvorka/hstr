"""Exercise the real ncurses UI using a PTY and disposable history files."""

import contextlib
import errno
import fcntl
import os
from pathlib import Path
import pty
import select
import signal
import struct
import subprocess
import sys
import tempfile
import termios
import time

import pyte


UP, DOWN = b"\x1bOA", b"\x1bOB"
PAGE_UP, PAGE_DOWN, DELETE = b"\x1b[5~", b"\x1b[6~", b"\x1b[3~"


class Terminal:
    def __init__(self, directory, options="", count=120):
        self.history = Path(directory) / ".bash_history"
        self.history.write_text("".join(f"entry-{i:03d}\n" for i in range(count)))
        self.fd, slave = pty.openpty()
        self.screen = pyte.Screen(80, 12)
        self.stream = pyte.ByteStream(self.screen)
        fcntl.ioctl(slave, termios.TIOCSWINSZ, struct.pack("HHHH", 12, 80, 0, 0))

        def session():
            os.setsid()
            fcntl.ioctl(slave, termios.TIOCSCTTY, 0)

        self.process = subprocess.Popen(
            [sys.argv[1]], stdin=slave, stdout=slave, stderr=subprocess.PIPE,
            preexec_fn=session,
            env={**os.environ, "HOME": directory, "HISTFILE": str(self.history),
                 "TERM": "xterm", "HSTR_TIOCSTI": "n",
                 "HSTR_CONFIG": "raw-history-view,no-confirm,monochromatic," + options},
        )
        os.close(slave)
        if count:
            self.wait_for(lambda: "HISTORY" in "\n".join(self.screen.display))
        else:
            self.process.wait(timeout=5)  # upstream exits immediately on an empty file
            assert self.process.returncode == 0

    def drain(self):
        while select.select([self.fd], [], [], 0.03)[0]:
            try:
                data = os.read(self.fd, 65536)
            except OSError as error:
                if error.errno == errno.EIO:
                    return
                raise
            if not data:
                return
            self.stream.feed(data)

    def wait_for(self, predicate):
        deadline = time.monotonic() + 10
        while time.monotonic() < deadline:
            self.drain()
            if predicate():
                return
            if self.process.poll() is not None:
                raise AssertionError(self.process.stderr.read().decode())
        raise AssertionError("Unexpected screen:\n" + "\n".join(self.screen.display))

    def send(self, keys):
        os.write(self.fd, keys)
        self.drain()

    def selected(self, number):
        name = f"entry-{number:03d}"
        return any(name in line and any(cell.reverse for cell in self.screen.buffer[y].values())
                   for y, line in enumerate(self.screen.display))

    def expect(self, number):
        self.wait_for(lambda: self.selected(number))

    def selected_row(self):
        return next(y for y, line in enumerate(self.screen.display)
                    if "entry-" in line
                    and any(cell.reverse for cell in self.screen.buffer[y].values()))

    def resize(self, rows, columns):
        self.screen.resize(rows, columns)
        fcntl.ioctl(self.fd, termios.TIOCSWINSZ, struct.pack("HHHH", rows, columns, 0, 0))
        os.kill(self.process.pid, signal.SIGWINCH)
        self.drain()

    def finish(self, key):
        self.send(key)
        self.process.wait(timeout=5)
        assert self.process.returncode == 0
        return self.process.stderr.read().decode()

    def close(self):
        if self.process.poll() is None:
            self.process.kill()
            self.process.wait()
        self.process.stderr.close()
        os.close(self.fd)


@contextlib.contextmanager
def terminal(options="", count=120):
    with tempfile.TemporaryDirectory() as directory:
        ui = Terminal(directory, options, count)
        try:
            yield ui
        finally:
            ui.close()


def test_layout(options, bottom=False):
    older, newer = (UP, DOWN) if bottom else (DOWN, UP)
    page_older, page_newer = (PAGE_UP, PAGE_DOWN) if bottom else (PAGE_DOWN, PAGE_UP)
    with terminal(options) as ui:
        ui.send(older * 25)
        ui.expect(95)  # well beyond the first nine visible entries
        ui.send(page_older)
        ui.expect(86)
        ui.send(page_newer)
        ui.expect(95)
        ui.resize(7, 60)
        ui.expect(95)
        ui.resize(18, 100)
        ui.expect(95)
        row_before_delete = ui.selected_row()
        ui.send(DELETE)
        ui.expect(94)
        assert ui.selected_row() == row_before_delete, "Deletion moved the selected row"
        ui.send(DELETE)
        ui.expect(93)
        assert ui.selected_row() == row_before_delete, "Repeated deletion moved the selected row"
        ui.finish(b"\x18")
        remaining = ui.history.read_text().splitlines()
        assert len(remaining) == 118 and "entry-095" not in remaining
        assert "entry-094" not in remaining and "entry-096" in remaining

    with terminal(options) as ui:
        ui.send(older * 200)
        ui.expect(0)  # clamp at the end instead of wrapping
        ui.send(DELETE)
        ui.expect(1)
        ui.send(newer * 200)
        ui.expect(119)
        assert "entry-119" in ui.finish(b"\t")

    with terminal(options) as ui:
        ui.send(older * 200 + newer * 4)
        ui.expect(4)
        row_before_delete = ui.selected_row()
        ui.send(DELETE)
        ui.expect(3)
        assert ui.selected_row() == row_before_delete, "Deletion shifted the last page"
        ui.finish(b"\x18")


for options, bottom in [("", False), ("prompt-bottom", True),
                        ("help-on-opposite-side", False),
                        ("prompt-bottom,help-on-opposite-side", True)]:
    test_layout(options, bottom)

with terminal() as ui:
    ui.send(DOWN * 30)
    ui.expect(90)
    ui.send(b"entry-02" + DOWN * 10)
    ui.expect(20)
    ui.send(b"\x15" + DOWN)  # clear filter, reset viewport
    ui.expect(119)
    ui.send(b"no-matches" + UP + PAGE_DOWN + DELETE)
    ui.send(b"\x15" + DOWN)
    ui.expect(119)
    ui.send(b"entry-002" + DOWN + DELETE + DELETE + UP + PAGE_DOWN)
    ui.finish(b"\x18")
    assert "entry-002" not in ui.history.read_text()

with terminal("prompt-bottom,regexp-matching") as ui:
    ui.send(b"entry-0[0-9][0-9]" + UP * 25)
    ui.expect(75)
    assert "entry-075" in ui.finish(b"\t")

with terminal(count=0) as ui:
    assert ui.process.returncode == 0

with terminal(count=1) as ui:
    ui.send(DOWN + DELETE + DELETE + UP + PAGE_DOWN)
    ui.finish(b"\x18")
    assert not ui.history.read_text().strip()

for favorites_count in (0, 150):
    with tempfile.TemporaryDirectory() as directory:
        Path(directory, ".hstr_favorites").write_text("".join(f"entry-{i:03d}\n" for i in range(1000, 1000+favorites_count)))
        ui=Terminal(directory)
        try:
            ui.send(b"\x1f" + DOWN * 200)
            if favorites_count:
                ui.expect(1149)
            ui.send(b"\x1f" + DOWN * 200)
            ui.expect(0)
            ui.send(b"\x1f" + DOWN * 200)
            ui.expect(0)
            ui.finish(b"\x07")
        finally:
            ui.close()

print("PASS: scrolling, paging, boundaries, resize, deletion, filtering, regex, empty lists, prompt layouts, view switching")
