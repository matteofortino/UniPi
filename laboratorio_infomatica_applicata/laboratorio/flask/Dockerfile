FROM ubuntu:latest

RUN apt update && apt install -y --no-install-recommends \
    python3 \
    python3-pip \
    && rm -rf /var/lib/apt/lists/*


WORKDIR /app
COPY ./requirements.txt .
RUN pip3 install --no-cache-dir --break-system-packages -r requirements.txt
COPY ./main.py . 

EXPOSE 5000

CMD ["python3", "./main.py"]


