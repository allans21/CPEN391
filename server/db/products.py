class Products():
    def __init__(self, id, name, price):
        self.id = id
        self.name = name
        self.price = price
    
def getPrice(db_conn, id):
    query = """SELECT PRICE
                   FROM PRODUCTS WHERE ID=%s"""
    curs = db_conn.cursor()
    curs.execute(query, [id])
    for row in curs:
        return row[0]
