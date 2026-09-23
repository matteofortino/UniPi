const form = document.querySelector(".form");
const text = document.querySelector(".text");

form.addEventListener("submit", async (e) => {
    e.preventDefault();

    const username = document.querySelector("#username").value.trim();
    const password = document.querySelector("#password").value.trim();

    // Pulisce eventuali messaggi precedenti
    text.textContent = "";
    text.style.color = "";

    if (!username || !password) {
        text.textContent = "Inserisci username e password.";
        text.style.color = "red";
        return;
    }

    try {
        const response = await fetch("../php/login.php", {
            method: "POST",
            headers: {
                "Content-Type": "application/json",
            },
            credentials: "include",
            body: JSON.stringify({ username, password }),
        });

        const result = await response.json();
        if (!response.ok) {
            text.textContent = result.message || "Errore durante il login.";
            text.style.color = "red";
            return;
        }

        text.textContent = result.message || "Login effettuato!";
        text.style.color = "green";

        // 🔁 Reindirizza automaticamente alla home dopo qualche secondo
        setTimeout(() => {
            window.location.href = "../";
        }, 1500);
    } catch (error) {
        console.error("Errore:", error);
        text.textContent = "Errore di connessione al server.";
        text.style.color = "red";
    }
});
