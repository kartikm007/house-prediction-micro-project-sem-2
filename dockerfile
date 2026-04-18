# Use an official Linux container with GCC (C++ compiler)
FROM gcc:latest

# Set the working directory
WORKDIR /app

# Copy all your files into the container
COPY . .

# Compile the C++ code for Linux (Notice: we use -lpthread instead of Windows' -lws2_32)
RUN g++ main.cpp -o server -lpthread

# Expose the port your app runs on
EXPOSE 8081

# Command to run the server
CMD ["./server"]