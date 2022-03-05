touch connection.txt

#Area
ns 1605042.tcl 250 40 20
echo -n "250" > connection.txt
awk -f parse.awk trace.tr
ns 1605042.tcl 500 40 20
echo -n "500" >> connection.txt
awk -f parse.awk trace.tr
ns 1605042.tcl 750 40 20
echo -n "750" >> connection.txt
awk -f parse.awk trace.tr
ns 1605042.tcl 1000 40 20
echo -n "1000" >> connection.txt
awk -f parse.awk trace.tr
ns 1605042.tcl 1250 40 20
echo -n "1250" >> connection.txt
awk -f parse.awk trace.tr
python graph.py 0

#Nodes
ns 1605042.tcl 500 20 20
echo -n "20" > connection.txt
awk -f parse.awk trace.tr
ns 1605042.tcl 500 40 20
echo -n "40" >> connection.txt
awk -f parse.awk trace.tr
ns 1605042.tcl 500 60 20
echo -n "60" >> connection.txt
awk -f parse.awk trace.tr
ns 1605042.tcl 500 80 20
echo -n "80" >> connection.txt
awk -f parse.awk trace.tr
ns 1605042.tcl 500 100 20
echo -n "100" >> connection.txt
awk -f parse.awk trace.tr
python graph.py 1

#Flows
ns 1605042.tcl 500 40 10
echo -n "10" > connection.txt
awk -f parse.awk trace.tr
ns 1605042.tcl 500 40 20
echo -n "20" >> connection.txt
awk -f parse.awk trace.tr
ns 1605042.tcl 500 40 30
echo -n "30" >> connection.txt
awk -f parse.awk trace.tr
ns 1605042.tcl 500 40 40
echo -n "40" >> connection.txt
awk -f parse.awk trace.tr
ns 1605042.tcl 500 40 50
echo -n "50" >> connection.txt
awk -f parse.awk trace.tr
python graph.py 2
