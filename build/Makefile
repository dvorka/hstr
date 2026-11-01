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

.DEFAULT_GOAL := help

#
# VARIABLES
#

# HSTR version - extracted from src/hstr.c
HSTR_VERSION := $(shell grep 'hstr version' ../src/hstr.c | sed 's/.*hstr version \\"\([^\\]*\)\\".*/\1/')

#
# CMD
#

DIFF := colordiff -u


#
# HELP
#

.PHONY: help
help: ## make targets help
	@echo " _   _ ____ _____ ____  "
	@echo "| | | / ___|_   _|  _ \\ "
	@echo "| |_| \\___ \\ | | | |_) |"
	@echo "|  _  |___) || | |  _ < "
	@echo "|_| |_|____/ |_| |_| \\_\\ $(HSTR_VERSION)"
	@echo ""
	@echo "HSTR: history suggest box - Swiss knife:"
	@grep -E '^[a-zA-Z0-9_-]+:.*?## .*$$' $(MAKEFILE_LIST) \
	| sort \
	| awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-30s\033[0m %s\n", $$1, $$2}'

#
# CODING
#

.PHONY: vibe
vibe:
	@ls -l ../.github/copilot*.md
	@cd .. && copilot --allow-all-tools --banner

#
# CLEAN
#

.PHONY: clean
clean: ## clean build artifacts
	@rm -vf ../hstr ../src/hstr
	@cd .. && make clean ; cd build

#
# BUILD
#

# sources - HSTR binary is rebuilt if any of them changes
HSTR_SOURCES := $(wildcard ../src/*.c ../src/include/*.h)
# automake/autoconf inputs - build is reconfigured if any of them changes
HSTR_BUILD_CONFIG := ../configure.ac ../Makefile.am ../src/Makefile.am ../man/Makefile.am

.PHONY: configure
configure: ## generate build files and configure HSTR build
	cd ./tarball && ./tarball-automake.sh && cd ../.. && ./configure

../src/Makefile: $(HSTR_BUILD_CONFIG)
	$(MAKE) configure

../src/hstr: ../src/Makefile $(HSTR_SOURCES)
	$(MAKE) -C .. all

.PHONY: build-dev
build-dev: ## clean, configure, and build HSTR
	$(MAKE) clean
	$(MAKE) configure
	$(MAKE) ../src/hstr
	@echo "HSTR $(HSTR_VERSION) built for development"

.PHONY: build
build: ../src/hstr ## build HSTR
	strip ../src/hstr

#
# RUN
#

.PHONY: run-dev
run-dev: build-dev ## run HSTR for development
	../src/hstr

run: ../src/hstr ## run HSTR
	../src/hstr

run-web: ## run HSTR web preview
	cd ../web && python3 -m http.server 8000

#
# TEST
#

.PHONY: test
test: ## run HSTR unit tests
	cd ../test && qmake hstr-unit-tests.pro && make clean all && ./hstr-unit-tests

.PHONY: test-memleaks
test-memleaks: ## run HSTR memory leak tests with Valgrind
	./test-memleaks.sh

.PHONY: test-memleaks-units
test-memleaks-units: ## run HSTR unit tests with Valgrind
	cd ../test && qmake hstr-unit-tests.pro && make clean all && valgrind --track-origins=yes --leak-check=full ./hstr-unit-tests

#
# INSTALL
#

.PHONY: install-personal
install-personal: build ## install HSTR for personal user
	@echo "Installing HSTR $(HSTR_VERSION) for personal user..."
	@rm -vf $(HOME)/bin/hstr
	@cp -vf ../src/hstr $(HOME)/bin
	@echo "HSTR $(HSTR_VERSION) installed to $(HOME)/bin"
	hstr --version

#
# package
#

.PHONY: .
releng-ubuntu: ## release launchpad package for Ubuntu
	@echo "= Diff ============================"
	$(DIFF) ./ubuntu/launchpad-make-all-releases.sh ~/p/hstr/launchpad/launchpad-make-all-releases.sh || true
	@echo "= Script ============================"
	cd ~/p/hstr/launchpad && ./launchpad-make-all-releases.sh
