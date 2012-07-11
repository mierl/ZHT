#!/usr/bin/python


import string
import sys
#import ast




if __name__ == '__main__':

    
    insert=0
    num_ins=0
    lookup=0
    num_lookup=0
    remove=0
    num_remove=0
    neighbor=0
    num_neighbor=0
    serverStart=0 
    num_server=0   
    datafile = open(sys.argv[1],"r")
    
    for line in datafile:
        words = line.split()
        if len(words) >= 8:
            if words[0] == 'Remove':
                remove += float(words[7])
                num_remove += 1
            if words[0] == 'Lookup':
                lookup += float(words[7])
                num_lookup += 1
            if words[0] == 'Inserted':
                insert += float(words[7])
                num_ins += 1
        if len(words) < 3:
            if words[0] == 'neighbors:':
                neighbor += int(words[1])
                num_neighbor += 1
            if words[0] == 'serverStart:':
                serverStart += int(words[1])
	        num_server += 1

    if num_neighbor != 0:
        print "Average neighbor collecting time: ", neighbor / num_neighbor
    if serverStart != 0:
        print "Average server registing time: ", serverStart / num_server	

    if num_ins != 0:
        print num_ins, "inserts,", "Average Insert: ", insert / num_ins
    if num_lookup !=0:
        print num_lookup, "lookups,", "Average Lookup: ", lookup / num_lookup
    if num_remove !=0:
        print num_remove, "removes,", "Average Remove: ", remove / num_remove

    print "Final result:"
    print insert / num_ins
    print lookup / num_lookup
    print remove / num_remove         
#    print "Bootstrap time:"
#    print neighbor / num_neighbor
#    print serverStart / num_server  
