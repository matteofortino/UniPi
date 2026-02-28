const form = document.querySelector(".form");
const text = document.querySelector(".text");

form.addEventListener("submit", async (e) => {
    e.preventDefault();

    const username = document.querySelector("#username").value.trim();
    const password = document.querySelector("#password").value.trim();

    // Pulisce i messaggi precedenti
    text.textContent = "";
    text.style.color = "";

    if (!username || !password) {
        text.textContent = "Inserisci username e password.";
        text.style.color = "red";
        return;
    }

    try {
        const response = await fetch("../php/signin.php", {
            method: "POST",
            headers: {
                "Content-Type": "application/json",
            },
            body: JSON.stringify({ username, password }),
        });

        const result = await response.json();
        if (!response.ok) {
            // http status !== 200
            text.textContent =
                result.message || "Errore durante la registrazione.";
            text.style.color = "red";
            return;
        }

        text.textContent = result.message || "Registrazione completata!";
        text.style.color = "green";

        // 🔁 Reindirizza automaticamente al login dopo qualche secondo
        setTimeout(() => {
            window.location.href = "../pages/login.php";
        }, 1500);
    } catch (error) {
        console.error("Errore:", error);
        text.textContent = "Errore di connessione al server.";
        text.style.color = "red";
    }
});
