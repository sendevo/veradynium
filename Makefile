# Variables
SOLVER_DIR=solver
GUI_DIR=gui
SERVER_DIR=server
VENV_DIR=$(SERVER_DIR)/venv
PYTHON=python3
PIP=pip

# -------------------------
# Version checks (Python, pip, GCC, Node.js, npm)
check-versions:
	@echo "Checking toolchain versions..."
	@python3 -c "import sys; exit(0) if sys.version_info >= (3,10) else exit(1)" \
		|| (echo '❌ Python 3.10+ required' && exit 1)
	@pip --version | grep -E 'pip (2[2-9]|[3-9][0-9])' >/dev/null \
		|| (echo '❌ pip >= 22 required' && exit 1)
	@gcc -dumpversion | awk -F. '{if ($$1 < 9) exit 1}' \
		|| (echo '❌ GCC >= 9 required (C++17 support)' && exit 1)
	@node --version | grep -E '^v1[89]\.|^v2[0-9]\.' >/dev/null \
		|| (echo '❌ Node.js 18+ required' && exit 1)
	@npm --version | grep -E '^[8-9]|1[0-9]' >/dev/null \
		|| (echo '❌ npm >= 8 required' && exit 1)
	@echo "✅ All versions OK"

# Default target
all: check-versions solver gui server

# -------------------------
# Build the solver binaries
solver:
	@echo "Building solver..."
	cd $(SOLVER_DIR) && make

# -------------------------
# Build the frontend (React + Vite)
gui:
	@echo "Building frontend..."
	cd $(GUI_DIR) && npm install
	cd $(GUI_DIR) && npm run build

# -------------------------
# Setup Python backend
server: venv install-deps

# Create virtualenv
venv:
	@if [ ! -d $(VENV_DIR) ]; then \
		echo "Creating virtual environment..."; \
		$(PYTHON) -m venv $(VENV_DIR); \
	fi

# Install Python dependencies
install-deps:
	@echo "Installing Python dependencies..."
	$(VENV_DIR)/bin/$(PIP) install -r $(SERVER_DIR)/requirements.txt

# -------------------------
# Run the server (development mode)
run:
	@echo "Starting FastAPI server..."
	$(VENV_DIR)/bin/uvicorn main:app --reload

# -------------------------
# Clean targets
clean:
	@echo "Cleaning solver..."
	cd $(SOLVER_DIR) && make clean
	@echo "Cleaning frontend..."
	cd $(GUI_DIR) && rm -rf node_modules dist
	@echo "Cleaning backend..."
	rm -rf $(VENV_DIR)

.PHONY: all solver gui server venv install-deps run clean
