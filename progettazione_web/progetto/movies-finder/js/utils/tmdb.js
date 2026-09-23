// utility class to interact with The Movie Database (TMDb) API
export default class Tmdb {
  constructor(apiKey) {
    this.API_KEY = apiKey;
    this.BASE_URL = "https://api.themoviedb.org/3";
  }

  async fetch(endpoint) {
    try {
      const response = await fetch(
        `${this.BASE_URL}${endpoint}api_key=${this.API_KEY}`
      );
      if (!response.ok)
        throw new Error(`HTTP error! status: ${response.status}`);
      else return await response.json();
    } catch (error) {
      return error;
    }
  }

  async searchMovies(query) {
    return await this.fetch(
      `/search/movie?query=${encodeURIComponent(query)}&`
    );
  }

  async getMovie(id) {
    return await this.fetch(`/movie/${id}?append_to_response=credits&`);
  }

  async getCollection(id) {
    return await this.fetch(`/collection/${id}?`);
  }

  static getImageURL(path, width = "w300") {
    return `https://image.tmdb.org/t/p/${width}/${path}`;
  }
}
