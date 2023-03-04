/***** OLTP #1 *****/

// retrieve warehouse
MATCH (w:Warehouse)
WHERE w.id = 1
RETURN w.tax;

// retrieve and update district
MATCH (d:District)
WHERE d.id = 10
SET d.next_o_id = toInteger(d.next_o_id) + 1
RETURN d.tax, d.next_o_id;

// retrieve customer
MATCH (c:Customer)
WHERE c.id = 99
RETURN c.discount, c.last, c.credit;

// insert order
MATCH (d:District)
WHERE d.id = 10
WITH d.next_o_id AS O_ID
CREATE (o:Order {id: 900})
SET o.entry_d = datetime(), o.carrier_id = 0, o.ol_cnt = O_ID, o.all_local = 1, o.new_order = 1
return o;

// retrieve item
MATCH (i:Item)
WHERE i.id = 1000
RETURN i.price, i.name, i.data;

// retrieve and update stock
MATCH (s:Stock)
WHERE s.id = 5000
WITH s,
	 CASE
	 	WHEN (toInteger(s.quantity) - 25) >= 10 THEN -25
		ELSE -25 + 91
	 END AS s_quantity_diff
SET s.quantity = toInteger(s.quantity) + toInteger(s_quantity_diff), s.ytd = toInteger(s.ytd) + 25, s.order_cnt = toInteger(s.order_cnt) + 1, s.remote_cnt = toInteger(s.remote_cnt) + 1
RETURN s.quantity, s.dist_03, s.data;

// insert orderline
CREATE (ol:OrderLine {id: 1})
SET ol.number = 1, ol.delivery_d = datetime("1973-01-01T18:40:32.142+0100"), ol.quantity = 25, ol.amount = 100, ol.dist_info = 'V4oiHgwyuUXLLr3X8ddUwaxV'
WITH ol
MATCH (o:Order)
WHERE o.id = 900
MATCH (s:Stock)
WHERE s.id = 5000
WITH o, ol, s
MERGE (o)-[r1:contains]->(ol)
MERGE (ol)-[r2:hasStock]->(s)
return r1, r2;


/***** OLTP #2 *****/

// retrieve and update warehouse
MATCH (w:Warehouse)
WHERE w.id = 2
SET w.ytd = toInteger(w.ytd) + 20
RETURN w.name, w.street_1, w.street_2, w.city, w.state, w.zip;

// retrieve and update district
MATCH (d:District)
WHERE d.id = 15
SET d.ytd = toInteger(d.ytd) + 20
RETURN d.name, d.street_1, d.street_2, d.city, d.state, d.zip, d.ytd;

// retrieve and update customer if (Y > 60)
MATCH (c:Customer)
WHERE c.id = 1
WITH c,
CASE c.credit
	WHEN "BC" THEN (toString(c.id) + toString(20) + c.data)
	ELSE c.data
END AS c_new_data
SET c.balance = toInteger(c.balance) - 20, c.ytd_payment = toInteger(c.ytd_payment) + 20, c.payment_cnt = toInteger(c.payment_cnt) + 1, c.data = c_new_data
RETURN c.first, c.middle, c.last, c.street_1, c.street_2, c.city, c.state, c.zip, c.phone, c.since, c.credit, c.credit_lim, c.discount, c.balance, c.data;

// retrieve and update customer else
MATCH (c:Customer)
WHERE c.last = 'BARBARBAR'
WITH c
ORDER BY c.first
LIMIT 63
WITH c
ORDER BY c.first DESC
LIMIT 1
WITH c,
CASE c.credit
	WHEN "BC" THEN (toString(c.id) + toString(20) + c.data)
	ELSE c.data
END AS c_new_data
SET c.balance = toInteger(c.balance) - 20, c.ytd_payment = toInteger(c.ytd_payment) + 20, c.payment_cnt = toInteger(c.payment_cnt) + 1, c.data = c_new_data
RETURN c.first, c.middle, c.last, c.street_1, c.street_2, c.city, c.state, c.zip, c.phone, c.since, c.credit, c.credit_lim, c.discount, c.balance, c.data;


/***** OLTP #3 *****/

// retrieve customer if (Y > 60)
MATCH (c:Customer)
WHERE c.id = 1
RETURN c.balance, c.first, c.middle, c.last;

// retrieve customer else
MATCH (c:Customer)
WHERE c.last = 'BARBARBAR'
WITH c
ORDER BY c.first
LIMIT 63
WITH c
ORDER BY c.first DESC
LIMIT 1
RETURN c.balance, c.first, c.middle, c.last;

// retrieve order
MATCH (c:Customer)-[r1:hasPlaced]->(o:Order)
WHERE c.id = 1
WITH c, o
ORDER BY o.id DESC
LIMIT 1
RETURN o.id, o.entry_d, o.carrier_id;

// retrieve orderline
MATCH (c:Customer)-[r1:hasPlaced]->(o:Order)
WHERE c.id = 1
WITH o
ORDER BY o.id DESC
LIMIT 1
MATCH (o:Order)-[r2:contains]->(ol:OrderLine)
RETURN ol.id, ol.quantity, ol.amount, ol.delivery_d;


/***** OLTP #4 *****/

// retrieve and update customer, order and orderlines
MATCH (d:District WHERE d.id = 10)-[r1:serves]->(c:Customer)-[r2:hasPlaced]->(o:Order WHERE o.new_order = '1')
WITH c, o
ORDER BY o.id
LIMIT 1
SET o.new_order = 0, o.carrier_id = 10
WITH c, o
MATCH (o)-[r3:contains]->(ol:OrderLine)
SET ol.delivery_d = datetime()
WITH c, sum(toInteger(ol.amount)) AS total_amount
SET c.balance = toInteger(c.balance) + total_amount, c.delivery_cnt = toInteger(c.delivery_cnt) + 1
return c;


/***** OLTP #5 *****/

// retrieve district
MATCH (d:District)
WHERE d.id = 20
WITH d, d.next_o_id AS O_ID
MATCH (d)-[r1:serves]->(c:Customer)-[r2:hasPlaced]->(o:Order WHERE o.id < toInteger(O_ID) AND o.id >= (toInteger(O_ID) - 20))-[r3:contains]->(ol:OrderLine)-[r4:hasStock]->(s:Stock WHERE s.quantity < 100)<-[r5:hasStock]-(i:Item)
RETURN count(DISTINCT i);
