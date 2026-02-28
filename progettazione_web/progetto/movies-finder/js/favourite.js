import Tmdb from "./utils/tmdb.js";
const container = document.querySelector("#favourites-container");

const tmdb = new Tmdb("fb8fe02ad0ed0ffbbd05fe5075a1c625");
window.addEventListener("load", async () => {
    const response = await fetch("../php/favourite.php", {
        credentials: "include",
    });
    const result = await response.json();
    const favourites = result.data.favourites;
    console.log(favourites);

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
});
