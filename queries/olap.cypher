/***** OLAP #1 *****/

MATCH (ol:OrderLine) WHERE ol.delivery_d > datetime('2007-01-02T00:00:00.000000+0000') 
RETURN SUM(ol.quantity) AS sum_qty, COUNT(ol) AS count_order, SUM(ol.amount) AS sum_amount, 
            SUM(ol.quantity) / toFloat(COUNT(ol)) AS avg_qty, SUM(ol.amount) / toFloat(COUNT(ol)) AS avg_amount, ol.number 
ORDER BY ol.number ASC;


/***** OLAP #2 *****/

CALL {
		MATCH (i2:Item)-[:hasStock]->(s2:Stock)-[:hasSupplier]->(sup2:Supplier)-[:isLocatedIn]->(n2:Nation)-[:isPartOf]->(r2:Region)
		WHERE r2.name =~ 'EUROP.*' AND i2.data =~ ".*b"
		RETURN MIN(s2.quantity) AS m_s_quantity, i2.id AS i_s_id
	}
MATCH (i:Item)-[:hasStock]->(s:Stock)-[:hasSupplier]->(sup:Supplier)-[:isLocatedIn]->(n:Nation)-[:isPartOf]->(r:Region)
WHERE r.name =~ 'EUROP.*' AND i.data =~ ".*b" AND i.id = i_s_id AND m_s_quantity = s.quantity
RETURN i.id, sup.name, n.name, i.name, sup.address, sup.phone, sup.comment
ORDER BY n.name, sup.name, i.id;


/***** OLAP #3 *****/

MATCH(c:Customer)-[:hasPlaced]->(o:Order)-[:contains]->(ol:OrderLine) 
WHERE c.state =~ 'A.*' AND o.entry_d > datetime('2007-01-02T00:00:00.000000') 
WITH SUM(ol.amount) AS revenue, o.entry_d AS o_entry_d, o.id AS o_id 
RETURN revenue, o_entry_d, o_id ORDER BY revenue DESC;


/***** OLAP #4 *****/

MATCH(o:Order)-[:contains]->(ol:OrderLine) 
WHERE o.entry_d >= datetime('2007-01-02T00:00:00.000000') AND o.entry_d < datetime('2012-01-02T00:00:00.000000') 
AND ol.delivery_d >= o.entry_d WITH o.ol_cnt AS o_ol_cnt, COUNT(*) AS order_count 
RETURN o_ol_cnt, order_count;


/***** OLAP #5 *****/

MATCH (n:Nation)<-[:isLocatedIn]-(c:Customer)-[:hasPlaced]->(o:Order)-[:contains]->(ol:OrderLine) 
MATCH(n)-[:isPartOf]->(r:Region) WHERE r.name = 'EUROPE' AND o.entry_d >= datetime('2007-01-02T00:00:00.000000') 
WITH SUM(ol.amount) AS revenue, n.name AS n_name 
RETURN n_name, revenue;


/***** OLAP #6 *****/

MATCH(ol:OrderLine)
WHERE ol.delivery_d >=datetime('1970-01-01T00:00:00.000+0000') AND ol.delivery_d < datetime('2020-01-01T00:00:00.000+0000')
AND ol.quantity >= 1 AND ol.quantity <= 100000
RETURN SUM(ol.amount) AS revenue;


/***** OLAP #7 *****/

MATCH (c:Customer)-[:hasPlaced]->(o:Order)-[:contains]->(ol:OrderLine)-[:hasStock]->(s:Stock)-[:hasSupplier]->(sup:Supplier) 
WHERE ol.delivery_d >= datetime('2007-01-02T00:00:00.000000') AND ol.delivery_d < datetime('2012-01-02T00:00:00.000000') 
MATCH (c)-[:isLocatedIn]->(n1:Nation) 
MATCH (sup)-[:isLocatedIn]->(n2:Nation) 
WITH substring(c.state, 0, 1) AS cust_state, n1.name AS cust_nation, n2.name AS sup_nation, o.entry_d.year AS l_year, 
    SUM(ol.amount) AS revenue 
WHERE (cust_nation = 'CAMBODIA' AND sup_nation = 'GERMANY') OR (cust_nation = 'GERMANY' AND sup_nation = 'CAMBODIA') 
RETURN cust_state, sup_nation, l_year, revenue 
ORDER BY sup_nation, cust_nation, l_year;


/***** OLAP #8 *****/

MATCH (c:Customer)-[:hasPlaced]->(o:Order)-[:contains]->(ol:OrderLine)-[:hasStock]->(s:Stock)<-[:hasStock]-(i:Item) 
MATCH (c)-[:isLocatedIn]->(n:Nation)-[:isPartOf]->(r:Region) 
MATCH (s)-[:hasSupplier]->(sup:Supplier)-[:isLocatedIn]->(n2:Nation)
WHERE i.id < 1000 AND i.data =~ '.*b' AND r.name = 'EUROPE' AND 
    o.entry_d >= datetime('2007-01-02T00:00:00.000000') AND o.entry_d < datetime('2012-01-02T00:00:00.000000') 
WITH o.entry_d.year AS l_year, SUM(CASE WHEN n2.name = 'GREECE' THEN ol.amount ELSE 0.0 END) AS mkt_share
RETURN l_year, mkt_share
ORDER BY l_year;


/***** OLAP #9 *****/

MATCH(o:Order)-[:contains]->(ol:OrderLine)-[:hasStock]->(s:Stock)<-[:hasStock]-(i:Item) 
MATCH(s)-[:hasSupplier]->(sup:Supplier)-[:isLocatedIn]->(n:Nation) 
WHERE i.data =~ '.*BB'
WITH n.name AS n_name, o.entry_d.year AS entry_d_year, SUM(ol.amount) AS sum_profit
RETURN n_name, entry_d_year, sum_profit
ORDER BY n_name, entry_d_year DESC;


/***** OLAP #10 *****/

MATCH(c:Customer)-[:hasPlaced]->(o:Order)-[:contains]->(ol:OrderLine) 
MATCH(c)-[:isLocatedIn]->(n:Nation) 
WHERE o.entry_d >= datetime('2007-01-02T00:00:00.000000') AND o.entry_d <= ol.delivery_d 
WITH c.id AS c_id, c.last AS c_last, c.city AS c_city, c.phone AS c_phone, n.name AS n_name, SUM(ol.amount) AS revenue
RETURN c_id, c_last, c_city, c_phone, n_name, revenue
ORDER BY revenue DESC;


/***** OLAP #11 *****/

CALL { 
		MATCH (s2:Stock)-[:hasSupplier]->(sup2:Supplier)-[:isLocatedIn]->(n2:Nation) 
		WHERE n2.name = 'GERMANY' 
		RETURN SUM(s2.order_cnt) * 0.005 AS order_count 
	} 
MATCH (i:Item)-[:hasStock]->(s:Stock)-[:hasSupplier]->(sup:Supplier)-[:isLocatedIn]->(n:Nation {name: 'Germany'})  
WITH order_count AS order_cnt, SUM(s.order_cnt) AS s_order_cnt, i.id AS i_id
WHERE s_order_cnt > order_cnt
RETURN i_id, s_order_cnt 
ORDER BY s_order_cnt;


/***** OLAP #12 *****/

MATCH(o:Order)-[:contains]->(ol:OrderLine) 
WHERE o.entry_d <= ol.delivery_d AND ol.delivery_d < datetime('2020-01-01T00:00:00.000000') 
WITH CASE
        WHEN o.carrier_id = 1 OR o.carrier_id = 2 THEN 1
        ELSE 0 END AS high_line,
    CASE 
        WHEN o.carrier_id <> 1 AND o.carrier_id<> 2 THEN 1
        ELSE 0 END AS low_line,
    o.ol_cnt AS o_ol_cnt 
RETURN o_ol_cnt, SUM(high_line) AS high_line_count, SUM(low_line) AS low_line_count
ORDER BY o_ol_cnt;


/***** OLAP #13 *****/

MATCH (c:Customer)-[:hasPlaced]->(o:Order) 
WHERE o.carrier_id > 8 WITH c.id AS c_id, COUNT(o.id) AS c_count 
RETURN c_count, COUNT(c_id) AS cust_dist 
ORDER BY cust_dist DESC, c_count DESC;


/***** OLAP #14 *****/

MATCH (ol:OrderLine)-[:hasStock]->(s:Stock)<-[:hasStock]-(i:Item) 
WITH 100.0 * SUM(CASE WHEN i.data =~ 'PR.*' THEN ol.amount ELSE 0 END) AS amount1, 
    1+SUM(ol.amount) AS amount2 
RETURN amount1/amount2;


/***** OLAP #15 *****/

CALL { 
		MATCH (ol:OrderLine)-[:hasStock]->(s:Stock)-[:hasSupplier]->(sup:Supplier) 
		WHERE ol.delivery_d >= datetime('2011-01-02T00:00:00.000000') 
		RETURN sup.id AS supplier_no, SUM(ol.amount) AS total_revenue 
	} WITH MAX(total_revenue) AS max_revenue, total_revenue AS supp_revenue, supplier_no AS supp_no 
MATCH(supp2:Supplier) WHERE supp2.id = supp_no AND supp_revenue = max_revenue 
RETURN supp2.id, supp2.name, supp2.address, supp2.phone, supp_revenue;


/***** OLAP #16 *****/

CALL { 
		MATCH(supp:Supplier) WHERE supp.comment =~ '.*bad.*' 
		RETURN supp.id AS supp_key 
	} WITH collect(DISTINCT supp_key) AS supp_keys 
MATCH(i:Item)-[:hasStock]->(s:Stock)-[:hasSupplier]->(supp2:Supplier) 
WHERE NOT (i.data =~ 'zz.*') AND NOT supp2.id IN supp_keys 
RETURN i.name, i.price, COUNT(DISTINCT supp2.id) AS supplier_count, substring(i.data, 0, 3) AS brand 
ORDER BY supplier_count DESC;


/***** OLAP #17 *****/

CALL { MATCH(i:Item)-[:hasStock]->(s:Stock)<-[:hasStock]-(ol:OrderLine) 
		WHERE i.data =~ '.*b' RETURN AVG(ol.quantity) AS avg_quantity, i.id AS i_id 
	} 
MATCH(ol2:OrderLine) 
WHERE ol2.id = i_id AND ol2.quantity < avg_quantity 
RETURN SUM(ol2.amount) / 2.0 AS avg_yearly;


/***** OLAP #18 *****/

MATCH (c:Customer)-[:hasPlaced]->(o:Order)-[:contains]->(ol:OrderLine) 
MATCH (ol:OrderLine)-[:hasStock]->(s:Stock)<-[:hasStock]-(w:Warehouse) 
WITH SUM(ol.amount) AS sum_amount, c.id AS c_id, o.id AS o_id, ol.cnt AS ol_cnt, o.entry_d AS o_entry_d, c.last AS c_last, w.id AS w_id 
WHERE sum_amount > 100 
RETURN sum_amount, c_id, o_entry_d, ol_cnt, o_id, c_last 
ORDER BY sum_amount DESC, o_entry_d;


/***** OLAP #19 *****/

MATCH (ol:OrderLine)-[:hasStock]->(s:Stock)<-[:hasStock]-(i:Item)
MATCH (ol:OrderLine)-[:hasStock]->(s2:Stock)<-[:hasStock]-(w:Warehouse)
WHERE ol.quantity >= 1 AND ol.quantity <= 10 AND i.price >= 1 AND i.price <= 400000 
    AND (i.data =~ '.*a' AND w.id IN [1,2,3]) OR (i.data =~ '.*b' AND w.id IN [1,2,4]) OR (i.data =~ '.*c' AND w.id IN [1,5,3]) 
RETURN SUM(ol.amount) AS revenue;


/***** OLAP #20 *****/

CALL { 
		MATCH(i:Item)-[:hasStock]->(s:Stock)<-[:hasStock]-(ol:OrderLine) 
		WHERE i.data =~'co.*' AND ol.delivery_d > datetime('2010-05-23T12:00:00') 
		WITH s AS s, s.quantity AS s_quantity, SUM(ol.quantity) AS sum_quantity 
		WHERE 2 * s_quantity > sum_quantity 
		RETURN s 
	} MATCH(s)-[:hasSupplier]->(supp:Supplier)-[:isLocatedIn]->(n:Nation {name: 'TAIWAN'})
RETURN supp.name, supp.address
ORDER BY supp.name;


/***** OLAP #21 *****/

MATCH(o:Order)-[:contains]->(ol1:OrderLine)-[:hasStock]->(s:Stock)-[:hasSupplier]->(sup:Supplier)-[:isLocatedIn]->(n:Nation) 
WHERE NOT EXISTS { 
    MATCH (o)-[:contains]->(ol2:OrderLine) WHERE ol2.delivery_d > ol1.delivery_d 
} AND ol1.delivery_d > o.entry_d AND n.name = 'GERMANY' 
RETURN sup.name, COUNT(*) AS numwait;


/***** OLAP #22 *****/

CALL {
		MATCH(c2:Customer)
		WHERE substring(c2.phone, 0, 1) IN ['1', '2', '3', '4', '5', '6', '7'] AND c2.balance > 0 
		RETURN AVG(c2.balance) AS avg_balance
	}
MATCH(c:Customer)
WHERE substring(c.phone, 0, 1) IN ['1', '2', '3', '4', '5', '6', '7'] AND c.balance > avg_balance AND 
    NOT exists((c)-[:hasPlaced]->(:Order))
RETURN substring(c.state, 0, 1) AS c_state, COUNT(*) AS numcust, SUM(c.balance) AS totacctbal;
