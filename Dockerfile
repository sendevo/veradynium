FROM python:3.11-slim AS solver-builder

WORKDIR /app/solver
RUN apt-get update \
    && apt-get install -y --no-install-recommends build-essential make g++ libgsl-dev \
    && rm -rf /var/lib/apt/lists/*

COPY solver/ ./
RUN make clean && make


FROM node:20-alpine AS gui-builder

WORKDIR /app/gui
COPY gui/package.json gui/package-lock.json ./
RUN npm ci
COPY gui/ ./
RUN npm run build -- --outDir dist


FROM python:3.11-slim AS runtime

ENV PYTHONUNBUFFERED=1
ENV PYTHONDONTWRITEBYTECODE=1

WORKDIR /app/server
RUN apt-get update \
    && apt-get install -y --no-install-recommends libgsl-dev libgomp1 \
    && rm -rf /var/lib/apt/lists/*

COPY server/requirements.txt ./requirements.txt
RUN pip install --no-cache-dir -r requirements.txt

COPY server/ ./
COPY --from=solver-builder /app/solver /app/solver
COPY --from=gui-builder /app/gui/dist /app/server/static

EXPOSE 8000
CMD ["uvicorn", "main:app", "--host", "0.0.0.0", "--port", "8000"]
