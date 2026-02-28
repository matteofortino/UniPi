import Tmdb from "./utils/tmdb.js";
const API_KEY = "fb8fe02ad0ed0ffbbd05fe5075a1c625";
const tmdb = new Tmdb(API_KEY);

const params = new URL(document.location.href).searchParams;
const friend = params.get("id");

const pageTitle = document.getElementsByTagName("h1")[0];
pageTitle.textContent = `${friend}'s favourite movies`;

const container = document.querySelector("#favourites-container");
console.log(friend);

async function getFavourites() {
    try {
        const response = await fetch(
            `../php/friend_favourite.php?id=${friend}`,
            {
                method: "GET",
                headers: {
                    "Content-Type": "application/json",
                },
                credentials: "include",
            },
        );
        const result = await response.json();

        const favourites = result.data.favourites;
        if (favourites == null) {
            console.log("Nessun film preferito");
            return;
        }

        if (favourites.length === 0) {
            container.innerHTML = "<p>No favourite items found.</p>";
            return;
        }
        favourites.forEach(async (favourite) => {
            const movie = await tmdb.getMovie(favourite.movie_id);
            if (!movie) return;
            if (!movie.poster_path) return;

            const movieEl = document.createElement("div");

            movieEl.classList.add("favourite-item");
            const img = document.createElement("img");
            img.src = Tmdb.getImageURL(movie.poster_path, "w200");
            const title = document.createElement("a");
            title.href = `movie.php?id=${movie.id}`;

            movieEl.appendChild(img);
            movieEl.appendChild(title);

            container.appendChild(movieEl);
        });
    } catch (e) {
        console.error("Error:", e);
    }
}

getFavourites();
