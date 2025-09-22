# Variables
SOLVER_DIR=solver
GUI_DIR=gui
SERVER_DIR=server
VENV_DIR=$(SERVER_DIR)/venv
PYTHON=python3
PIP=pip

# Default target
all: solver gui server

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
