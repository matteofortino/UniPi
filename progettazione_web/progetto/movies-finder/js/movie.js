import Tmdb from "./utils/tmdb.js";

// key for themoviedb.org API
const API_KEY = "fb8fe02ad0ed0ffbbd05fe5075a1c625";
const tmdb = new Tmdb(API_KEY);

const title = document.querySelector("#title");
const overview = document.querySelector("#overview");
const poster = document.querySelector("#img");
const releaseDate = document.querySelector("#release-date");
const genresEl = document.querySelector("#genres");
const credits = document.querySelector("#credits");
const collectionEl = document.querySelector("#collection-container");
const duration = document.querySelector("#duration");

const params = new URL(document.location.href).searchParams;
const movieId = params.get("id");

window.addEventListener("load", async () => {
    const movie = await tmdb.getMovie(movieId);
    console.log(movie);

    if (movie.belongs_to_collection) {
        const collection = await tmdb.getCollection(
            movie.belongs_to_collection.id,
        );
        console.log(collection);
        collectionEl.setAttribute("class", "collection");
        collection.parts.forEach((part) => {
            const div = document.createElement("div");
            const img = document.createElement("img");
            const title = document.createElement("a");
            img.src = Tmdb.getImageURL(part.poster_path);

            title.href = `/movie.php?id=${part.id}`;
            title.innerText = part.title;

            div.append(img, title);
            collectionEl.append(div);

            div.setAttribute("class", "collection-item");
            img.setAttribute("class", "collection-img");
            title.setAttribute("class", "collection-title");
        });
    }
    genresEl.setAttribute("class", "genres");
    movie.genres.forEach((genre) => {
        const genres = document.createElement("span");
        genres.innerText = genre.name + ", ";
        genresEl.append(genres);
    });
    genresEl.innerText = genresEl.innerText.slice(0, -1); // remove last comma

    let count = 0;
    movie.credits.cast.some((member) => {
        if (!member.profile_path) return;
        const div = document.createElement("div");
        const img = document.createElement("img");
        const span = document.createElement("span");
        span.innerText = member.name + ", " + member.character;
        img.src = Tmdb.getImageURL(member.profile_path, "w200");

        div.append(img, span);
        div.setAttribute("class", "credit");
        img.setAttribute("class", "credit-img");
        span.setAttribute("class", "credit-text");
        credits.append(div);
        if (++count === 12) return true;
    });
    poster.src = Tmdb.getImageURL(movie.poster_path);

    releaseDate.innerText = movie.release_date.split("-")[0];
    title.innerText = movie.title;
    overview.innerText = movie.overview;
    duration.innerText = movie.runtime + " min";
});
