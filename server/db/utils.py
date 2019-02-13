import mysql.connector

def get_connection():
    return mysql.connector.connect(
        host = "rds-cpen391-group5.c4bbl9mziasa.ca-central-1.rds.amazonaws.com",
        user = "Group5",
        database = "CPEN391_G5",
        passwd = "CPEN391G5",
        port = "3306"
    )
