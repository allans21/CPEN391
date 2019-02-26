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

def getPriceByStock(db_conn, stock_id):
    query = """SELECT PRODUCTS.PRICE
                   FROM STOCK
                   LEFT OUTER JOIN PRODUCTS ON PRODUCTS.ID = STOCK.PRODUCTID
                   WHERE STOCK.ID=%s """
    curs = db_conn.cursor()
    curs.execute(query, [stock_id])
    for row in curs:
        return row[0]
