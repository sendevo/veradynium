export const api = (path) => path; // path like "/api/upload"

export const api_call_timeout = 15000; // ms

// Line-of-sight and Fresnel zone constants
export const US915_LORA_LAMBDA = 0.327642031; // in meters (for 915 MHz)
export const EU860_LORA_LAMBDA = 0.345383016; // in meters (for 868 MHz)
export const FRESNEL_ZONE_CLEARANCE = 0.6; // 60% clearance