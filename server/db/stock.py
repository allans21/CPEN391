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
