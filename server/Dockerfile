# an official Python runtime as a base image
FROM python:3.7-slim

# Set the working directory to /app
WORKDIR /app

# Copy the current directory contents into the container at /app
ADD . /app

# Install any needed packages specified in requirements.txt
RUN pip install -r requirements.txt

# Make port 5000 available to the world outside this container
EXPOSE 5000

# Define environment variable
ENV NAME Vending
ENV AUTHOR Aidan
ENV FLASK_ENV development

#RUN app.py when the container launches
CMD ["python","app.py"]

