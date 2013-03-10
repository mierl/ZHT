 #
 # Copyright (C) 2010-2020 DatasysLab@iit.edu(http://datasys.cs.iit.edu/index.html).
 #      Director: Ioan Raicu(iraicu@cs.iit.edu)
 #	 
 # This file is part of ZHT library(http://datasys.cs.iit.edu/projects/ZHT/index.html).
 #      Ioan Raicu(iraicu@cs.iit.edu),
 #      Tonglin Li(tli13@hawk.iit.edu) with nickname tony,
 #      Xiaobing Zhou(xzhou40@hawk.iit.edu) with nickname xiaobingo.
 # 
 # The ZHT Library is free software; you can redistribute it and/or
 # modify it under the terms of the GNU Lesser General Public
 # License as published by the Free Software Foundation; either
 # version 2.1 of the License, or (at your option) any later version.
 #
 # The ZHT Library is distributed in the hope that it will be useful,
 # but WITHOUT ANY WARRANTY; without even the implied warranty of
 # MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 # Lesser General Public License for more details.
 #
 # You should have received a copy of the GNU Lesser General Public
 # License along with the ZHT Library; if not, write to the 
 # Data-Intensive Distributed Systems Laboratory, 10 W. 31st Street,
 # Stuart Building, Room 003B, Chicago, IL 60616 USA.
 #
 # run.sh
 #
 #  Created on: Aug 2, 2012
 #      Author: tony, xiaobingo
 #

gcc -g template_usage_test.cpp template_usage.cpp -o template_usage -lstdc++

gcc -g cmpl_usage_test.cpp cmpl_usage.cpp -o cmpl_usage -lstdc++


gcc -g inline_usage_test.cpp inline_usage.cpp -o inline_usage -lstdc++


gcc -g static_member_test.cpp static_member.cpp  -o static_member -lstdc++


gcc -g const_usage_test.cpp const_usage.cpp  -o const_usage -lstdc++

gcc -g vector_trans.cpp -o vector_trans -lstdc++

gcc -g vector_array.cpp -o vector_array -lstdc++

gcc -g main.cpp -o main -lstdc++


gcc -g file_ftruncate.cpp -o file_ftruncate -lstdc++ -lc


gcc -g ImplicitConop.cpp -o implicitcon_op -lstdc++ -lc

gcc -g socket_hostname.cpp -o socket_hostname -lstdc++ -lc

gcc -g strlensizeof.cpp -o strlensizeof -lstdc++ -lc

gcc -g array_as_pointer.cpp -o array_as_pointer -lstdc++ -lc

gcc -g conv_fromString.cpp -o conv_fromString -lstdc++ -lc

gcc -g set_test.cpp -o set_test -lstdc++ -lc

gcc -g const_ref.cpp -o const_ref -lstdc++ -lc

gcc -g fork_process.cpp -o fork_process -lstdc++ -lc

gcc -g stream_test.cpp -o stream_test -lstdc++ -lc

gcc -g udtfile_test.cpp -o udtfile_test -ludt -lpthread




