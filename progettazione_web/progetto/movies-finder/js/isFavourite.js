const params = new URL(document.location.href).searchParams;
const movieId = params.get("id");
const button = document.querySelector(".star");
const checkbox = document.querySelector("#favourite-checkbox");

window.addEventListener("load", async () => {
    try {
        const response = await fetch(`../php/is_favourite.php?id=${movieId}`, {
            credentials: "include",
        });

        const result = await response.json();
        if (!response.ok) {
            console.error(
                result.message || "Errore durante il recupero dei preferiti.",
            );
            return;
        }

        checkbox.checked = result.data.isFavourite;
    } catch (error) {
        console.error("Errore:", error);
    }
});

checkbox.addEventListener("change", async () => {
    console.log("changed...");

    try {
        const response = await fetch("../php/is_favourite.php", {
            method: "POST",
            headers: {
                "Content-Type": "application/json",
            },
            credentials: "include",
            body: JSON.stringify({ movieId, toAdd: checkbox.checked }),
        });

        const result = await response.json();
        if (!response.ok) {
            console.error(
                result.message || "Errore durante l'aggiunta ai preferiti.",
            );
            return;
        }

        console.log(result.message || "Film aggiunto ai preferiti!");
    } catch (error) {
        console.error("Errore:", error);
    }
});
