# Step 1: Use a Python base image from Docker Hub
FROM python:3.9-slim

# Install make and other build essentials
RUN apt-get update && apt-get install -y \
    g++ \
    make \
    && apt-get clean

# Set the working directory in the container to /app
WORKDIR /app

# Copy the requirements.txt file into the container
COPY requirements.txt .

# Install the required dependencies inside the container
RUN pip install --no-cache-dir -r requirements.txt

# Copy the rest of the application code into the container
COPY . .

# set the working directory in /app/src
WORKDIR /app/src

# Install quality
RUN make

WORKDIR /app

# Expose port 5000 (the default port for Flask)
EXPOSE 5000

# Step 7: Specify the command to run the app
CMD ["python", "app.py"]
