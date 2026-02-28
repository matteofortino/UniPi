import Tmdb from "./js/utils/tmdb.js";
const searchbar = document.querySelector("#search");
const button = document.querySelector("#button");
const container = document.querySelector(".movies-container");

// key for themoviedb.org API
const API_KEY = "fb8fe02ad0ed0ffbbd05fe5075a1c625";
const tmdb = new Tmdb(API_KEY);

searchbar.addEventListener("input", debounce(handleInput, 500));
button.addEventListener("click", handleInput);
async function handleInput(event) {
    //event.preventDefault();

    if (!searchbar.value) return;

    container.innerText = null;

    const { results: movies } = await tmdb.searchMovies(searchbar.value);
    console.log(movies);
    movies.forEach((movie) => {
        if (!movie.poster_path) return;
        const poster = document.createElement("img");
        const currentMovieContainer = document.createElement("div");
        const title = document.createElement("a");
        title.href = `pages/movie.php?id=${movie.id}`;
        title.innerText = movie.title;

        currentMovieContainer.href = `pages/movie.php?id=${movie.id}`;
        poster.addEventListener("click", () => {
            window.location.href = `pages/movie.php?id=${movie.id}`;
        });
        currentMovieContainer.addEventListener("click", () => {
            window.location.href = `pages/movie.php?id=${movie.id}`;
        });
        poster.src = Tmdb.getImageURL(movie.poster_path, "w200");
        currentMovieContainer.append(poster, title);

        currentMovieContainer.setAttribute("class", "movie");
        title.setAttribute("class", "movie-title");
        container.append(currentMovieContainer);
    });
}

function debounce(callback, delay) {
    let timer;
    return function () {
        clearTimeout(timer);
        timer = setTimeout(() => {
            callback();
        }, delay);
    };
}
