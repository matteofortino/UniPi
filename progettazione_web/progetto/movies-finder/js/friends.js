const pendingContainer = document.querySelector("#pending-requests");
const friendsContainer = document.querySelector("#friends-container");

const input = document.getElementById("user-search");
const dropdown = document.getElementById("dropdown");
const sendButton = document.getElementById("send-button");
const sentRequest = document.getElementById("sent-requests");

let username = "";
window.addEventListener("load", async () => {
    try {
        const resposne = await fetch("../php/get_username.php");
        const result = await resposne.json();
        if (!resposne.ok) {
            console.log(result.message || "Errore nel recupero dell' username");
            return;
        }

        username = result.data.username;
        console.log(username);
    } catch (e) {
        console.error("Error:", e);
    }

    // mostra gli amici
    try {
        const response = await fetch("../php/friend.php", {
            method: "POST",
            headers: {
                "Content-Type": "application/json",
            },
            credentials: "include",
            body: JSON.stringify({
                receiver: null,
                action: "get_friends",
            }),
        });

        const result = await response.json();
        if (!response.ok) {
            console.error(
                result.message || "Errore durante l'aggiunta ai preferiti.",
            );
            return;
        }
        const friends = result.data.friends;
        console.log(friends);
        friends.forEach((friend) => {
            const el = document.createElement("div");
            const text = document.createElement("span");
            const date = document.createElement("span");
            const rejectButton = document.createElement("button");
            text.textContent =
                friend.receiver == username ? friend.sender : friend.receiver;

            rejectButton.id = "reject-button";
            rejectButton.textContent = "Elimina -";
            rejectButton.addEventListener("click", deleteFriend);
            date.textContent = "Since: " + friend.created_at.split(" ")[0];

            el.append(text, date, rejectButton);
            el.addEventListener("click", showFriendsFavourites);
            friendsContainer.append(el);
        });
    } catch (error) {
        console.error(error);
    }
    // mostra le richeste di amicizia
    try {
        const response = await fetch("../php/friend.php", {
            method: "POST",
            headers: {
                "Content-Type": "application/json",
            },
            credentials: "include",
            body: JSON.stringify({
                receiver: null,
                action: "get_requests",
            }),
        });

        const result = await response.json();
        if (!response.ok) {
            console.error(
                result.message || "Errore durante l'aggiunta ai preferiti.",
            );
            return;
        }
        const requests = result.data.requests;
        console.log(requests);

        requests.forEach((request) => {
            const el = document.createElement("div");
            const addButton = document.createElement("button");
            const rejectButton = document.createElement("button");
            const text = document.createElement("span");

            text.textContent =
                request.receiver == username
                    ? request.sender
                    : request.receiver;

            addButton.id = "add-button";
            addButton.textContent = "Aggiungi +";
            addButton.addEventListener("click", addFriend);

            rejectButton.id = "reject-button";
            rejectButton.textContent = "Elimina -";
            rejectButton.addEventListener("click", deleteFriend);

            el.append(text, addButton, rejectButton);
            pendingContainer.append(el);
        });
    } catch (error) {
        console.error(error);
    }
    // mostra le richesta in attesa di risposta
    try {
        const pending_response = await fetch("../php/friend.php", {
            method: "POST",
            headers: {
                "Content-Type": "application/json",
            },
            credentials: "include",
            body: JSON.stringify({
                receiver: null,
                action: "pending",
            }),
        });

        const pending_result = await pending_response.json();
        if (!pending_response.ok) {
            console.error(
                pending_result.message ||
                    "Errore durante l'aggiunta ai preferiti.",
            );
            return;
        }
        const pendings = pending_result.data.pendings;
        pendings.forEach((pending) => {
            const el = document.createElement("div");
            el.textContent = "Richesta inviata a " + pending.receiver;
            sentRequest.append(el);
        });
        //location.reload();
    } catch (error) {
        console.error(error);
    }
});

async function addFriend(e) {
    const friend = e.target.parentElement.children[0].textContent;

    const response = await fetch("../php/friend.php", {
        method: "POST",
        headers: {
            "Content-Type": "application/json",
        },
        credentials: "include",
        body: JSON.stringify({
            receiver: friend,
            action: "add",
        }),
    });

    const result = await response.json();
    if (!response.ok) {
        console.error(
            result.message || "Errore durante l'aggiunta ai preferiti.",
        );
        return;
    }
    location.reload();
}
async function deleteFriend(e) {
    const friend = e.target.parentElement.children[0].textContent;

    const response = await fetch("../php/friend.php", {
        method: "POST",
        headers: {
            "Content-Type": "application/json",
        },
        credentials: "include",
        body: JSON.stringify({
            receiver: friend,
            action: "delete",
        }),
    });

    const result = await response.json();
    if (!response.ok) {
        console.error(
            result.message || "Errore durante l'aggiunta ai preferiti.",
        );
        return;
    }
    location.reload();
}

input.addEventListener("input", debounce(handleInput, 400));

async function handleInput() {
    console.log("scrivendo..");

    const query = input.value.trim();
    if (!query) {
        dropdown.style.display = "none";
        dropdown.innerHTML = "";
        return;
    }

    try {
        const response = await fetch("../php/user.php", {
            credentials: "include",
        });
        const result = await response.json();

        const users = result.data.users;

        dropdown.innerHTML = "";
        if (users.length === 0) {
            dropdown.style.display = "none";
            return;
        }

        const filtered = users.filter(
            (u) => u.username.startsWith(query) && u.username != username,
        );

        console.log(filtered.length);
        if (!filtered.length) {
            const div = document.createElement("div");
            div.textContent = "Nessun utente trovato";
            dropdown.append(div);
            dropdown.style.display = "block";
            return;
        }
        filtered.forEach((u) => {
            const div = document.createElement("div");
            div.textContent = u.username;
            div.addEventListener("click", () => {
                input.value = u.username;
                dropdown.style.display = "none";
            });
            dropdown.append(div);
        });

        dropdown.style.display = "block";
    } catch (err) {
        console.error(err);
    }
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

sendButton.addEventListener("click", async () => {
    const name = document.getElementById("user-search").value;
    console.log(name);

    const response = await fetch("../php/friend.php", {
        method: "POST",
        headers: {
            "Content-Type": "application/json",
        },
        credentials: "include",
        body: JSON.stringify({
            receiver: name,
            action: "send",
        }),
    });

    const result = await response.json();
    if (!response.ok) {
        console.error(
            result.message || "Errore durante l'aggiunta ai preferiti.",
        );
        return;
    }
    location.reload();
});

function showFriendsFavourites(event) {
    let friend = event.target.children[0].textContent;
    console.log(friend);

    window.location.href = `http://localhost/movies-finder/pages/friend_favourites.php?id=${friend}`;
}
