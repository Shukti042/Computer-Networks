# simulator
set ns [new Simulator]


# ======================================================================
# Define options

set val(chan)         Channel/WirelessChannel  ;# channel type
set val(prop)         Propagation/TwoRayGround ;# radio-propagation model
set val(ant)          Antenna/OmniAntenna      ;# Antenna type
set val(ll)           LL                       ;# Link layer type
set val(ifq)          CMUPriQueue              ;# Interface queue type
set val(ifqlen)       50                       ;# max packet in ifq
set val(netif)        Phy/WirelessPhy          ;# network interface type
set val(mac)          Mac/802_11               ;# MAC type
set val(rp)           DSR                      ;# ad-hoc routing protocol 
set val(nn)           [lindex $argv 1]         ;# number of mobilenodes
# =======================================================================
#Area
set area [lindex $argv 0]
# trace file
set trace_file [open trace.tr w]
$ns trace-all $trace_file

# nam file
set nam_file [open animation.nam w]
$ns namtrace-all-wireless $nam_file $area $area

# topology: to keep track of node movements
set topo [new Topography]
$topo load_flatgrid $area $area ;# $area m x $area m area


# general operation director for mobilenodes
create-god $val(nn)


# node configs
# ======================================================================

# $ns node-config -addressingType flat or hierarchical or expanded
#                  -adhocRouting   DSDV or DSR or TORA
#                  -llType	   LL
#                  -macType	   Mac/802_11
#                  -propType	   "Propagation/TwoRayGround"
#                  -ifqType	   "Queue/DropTail/PriQueue"
#                  -ifqLen	   50
#                  -phyType	   "Phy/WirelessPhy"
#                  -antType	   "Antenna/OmniAntenna"
#                  -channelType    "Channel/WirelessChannel"
#                  -topoInstance   $topo
#                  -energyModel    "EnergyModel"
#                  -initialEnergy  (in Joules)
#                  -rxPower        (in W)
#                  -txPower        (in W)
#                  -agentTrace     ON or OFF
#                  -routerTrace    ON or OFF
#                  -macTrace       ON or OFF
#                  -movementTrace  ON or OFF

# ======================================================================

$ns node-config -adhocRouting $val(rp) \
                -llType $val(ll) \
                -macType $val(mac) \
                -ifqType $val(ifq) \
                -ifqLen $val(ifqlen) \
                -antType $val(ant) \
                -propType $val(prop) \
                -phyType $val(netif) \
                -topoInstance $topo \
                -channelType $val(chan) \
                -agentTrace ON \
                -routerTrace ON \
                -macTrace OFF \
                -movementTrace OFF

# create nodes
for {set i 0} {$i < $val(nn) } {incr i} {
    set node($i) [$ns node]
    $node($i) random-motion 0       ;# disable random motion

    $node($i) set X_ [expr int(1+(rand() * ($area-1)))]
    $node($i) set Y_ [expr int(1+(rand() * ($area-1)))]
    $node($i) set Z_ 0

    $ns initial_node_pos $node($i) 20
    set destx [expr int(1+(rand() * ($area-1)))]
    set desty [expr int(1+(rand() * ($area-1)))]
    $ns at 0 "$node($i) setdest  $destx $desty [expr int(1+(rand() * 4 ))]"
} 





# Traffic
set val(nf)         [lindex $argv 2]                ;# number of flows
#shuffle nodes
for {set i 0} {$i<$val(nn)} {incr i} {set nums($i) $i}
  for {set i 0} {$i<$val(nn)} {incr i} {
    set j [expr {int(rand()*$val(nn))}]
    set temp $nums($j)
    set nums($j) $nums($i)
    set nums($i) $temp
  }
set seq 0

for {set i 0} {$i < $val(nf)} {incr i} {
    set seqincr [expr {$i%$val(nn)}]
    if {$seqincr == 0} {
        incr seq
    }
    set srcindex $seqincr
    set dstindex [expr {($i+$seq)%$val(nn)}]
    set src $nums($srcindex)
    set dest $nums($dstindex)

    # Traffic config
    # create agent
    set udp [new Agent/UDP]
    set null [new Agent/Null]

    # attach to nodes
    $ns attach-agent $node($src) $udp
    $ns attach-agent $node($dest) $null

    # connect agents
    $ns connect $udp $null
    $udp set fid_ $i

    # Traffic generator
    set cbr [new Application/Traffic/CBR]

    # attach to agent
    $cbr attach-agent $udp
    
    # start traffic generation
    $ns at 1.0 "$cbr start"
}



# End Simulation

# Stop nodes
for {set i 0} {$i < $val(nn)} {incr i} {
    $ns at 50.0 "$node($i) reset"
}

# call final function
proc finish {} {
    global ns trace_file nam_file
    $ns flush-trace
    close $trace_file
    close $nam_file
}

proc halt_simulation {} {
    global ns
    puts "Simulation ending"
    $ns halt
}

$ns at 50.0001 "finish"
$ns at 50.0002 "halt_simulation"




# Run simulation
puts "Simulation starting"
$ns run

