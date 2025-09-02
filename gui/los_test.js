(async () => {
    // Use relative path if frontend and API are served by the same FastAPI server
    const API_BASE = "http://127.0.0.1:8000";

    const payload = {
        p1: {
            lat: -45.825412,
            lon: -67.45874,
            height_m: 2.0
        },
        p2: {
            lat: -45.82915,
            lon: -67.45874,
            height_m: 2.5
        }
    };

    try {
        const res = await fetch(`${API_BASE}/api/los`, {
            method: "POST",
            headers: {
                "Content-Type": "application/json"
            },
            body: JSON.stringify(payload)
        });

        if (!res.ok) {
            const text = await res.text();
            console.error("Error response:", res.status, text);
            return;
        }

        const data = await res.json();
        console.log("LOS response:", JSON.stringify(data, null, 2));
    } catch (err) {
        console.error("Fetch failed:", err);
    }
})();
