import { defineConfig } from 'vite'
import react from '@vitejs/plugin-react'

// https://vite.dev/config/
export default defineConfig({
    plugins: [react()],
    build: {
        outDir: '../server/static', // relative to your vite.config.js location
        emptyOutDir: true, // clear the folder before each build
    },
    base: '/static/', // keep assets under /static in production
    server: {
        proxy: {
            '/api': { // proxy only API calls in dev
                target: 'http://127.0.0.1:8000',
                changeOrigin: true,
            }
        }
    }
})