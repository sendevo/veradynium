import { defineConfig } from 'vite'
import react from '@vitejs/plugin-react'

// https://vite.dev/config/
export default defineConfig({
  plugins: [react()],
  build: {
    outDir: '../server/static',  // relative to your vite.config.js location
    emptyOutDir: true,           // clear the folder before each build
  }
})
