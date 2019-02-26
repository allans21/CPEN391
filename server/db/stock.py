class Stock():
    def __init__(self, id, vending_id, product_id, quantity, slot):
        self.id = id
        self.vending_id = vending_id
        self.product_id = product_id
        self.quantity = quantity
        self.slot = slot


def get_stock(db_conn, vending_id):
    query = """select ID, VENDINGID, PRODUCTID, QUANTITY, SLOT from STOCK where VENDINGID=%s ORDER BY SLOT"""
    curs = db_conn.cursor()
    curs.execute(query, [vending_id])
    return [Stock(*row) for row in curs]


def getInventory(db_conn, vending_id):
    query = """SELECT STOCK.ID, STOCK.QUANTITY, STOCK.SLOT, PRODUCTS.NAME, PRODUCTS.PRICE 
                 FROM STOCK
                 LEFT JOIN PRODUCTS ON PRODUCTS.ID = STOCK.PRODUCTID
                 WHERE STOCK.VENDINGID = %s
                 ORDER BY STOCK.SLOT """
    curs = db_conn.cursor()
    curs.execute(query, [vending_id])
    return [{
        'stock_id': stock_id, 'quantity': quantity,
        'slot': slot, 'name': name, 'price': price
    } for stock_id, quantity, slot, name, price in curs]

def updateStock(db_conn, product_id, vending_id):
    query = """UPDATE STOCK SET QUANTITY = QUANTITY-1 WHERE (VENDINGID = %s AND  PRODUCTID = %s)"""
    curs = db_conn.cursor()
    curs.execute(query, [vending_id, product_id])
    db_conn.commit()

def getStockById(db_conn, product_id, vending_id):
    query = """select QUANTITY from STOCK where VENDINGID=%s AND PRODUCTID = %s """
    curs = db_conn.cursor()
    curs.execute(query, [vending_id, product_id])
    for row in curs:
        return row[0]