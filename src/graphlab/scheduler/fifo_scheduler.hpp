/*
* Copyright (C) 2015 Dato, Inc.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Affero General Public License as
* published by the Free Software Foundation, either version 3 of the
* License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Affero General Public License for more details.
*
* You should have received a copy of the GNU Affero General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/**
 * Copyright (c) 2009 Carnegie Mellon University.
 *     All rights reserved.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing,
 *  software distributed under the License is distributed on an "AS
 *  IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 *  express or implied.  See the License for the specific language
 *  governing permissions and limitations under the License.
 *
 * For more about this software visit:
 *
 *      http://www.graphlab.ml.cmu.edu
 *
 */


#ifndef GRAPHLAB_FIFO_SCHEDULER_HPP
#define GRAPHLAB_FIFO_SCHEDULER_HPP

#include <algorithm>
#include <queue>

#include <graph/graph_basic_types.hpp>
#include <parallel/pthread_tools.hpp>
#include <parallel/atomic.hpp>

#include <random/random.hpp>
#include <graphlab/scheduler/ischeduler.hpp>
#include <util/dense_bitset.hpp>

#include <graphlab/options/graphlab_options.hpp>

namespace graphlab {

  /**
   * \ingroup group_schedulers
   *
   * This class defines a multiple queue approximate fifo scheduler.
   * Each processor has its own in_queue which it puts new tasks in
   * and out_queue which it pulls tasks from.  Once a processors
   * in_queue gets too large, the entire queue is placed at the end of
   * the shared master queue.  Once a processors out queue is empty it
   * grabs the next out_queue from the master.
   */
  class fifo_scheduler : public ischeduler {
  
  public:

    typedef std::deque<lvid_type> queue_type;

  private:

    // a bitset denoting if a vertex is scheduled
    dense_bitset vertex_is_scheduled;
    // a collection of FIFO queues
    std::vector<queue_type> queues;
    // a parallel datastructure to queues containing all the locks
    std::vector<padded_simple_spinlock>   locks;
    // the index of the queue currently accessed by a given CPU
    // when used, this is modded so that it ranges from 0 to multi - 1
    std::vector<size_t>   current_queue; 


    // the number of CPUs
    size_t ncpus;
    // The queue to CPU ratio
    size_t multi;
    // the number of vertices in the graph
    size_t num_vertices;
    
    
  
    void set_options(const graphlab_options& opts); 

    // Initializes the internal datastructures
    void initialize_data_structures();
  public:

    fifo_scheduler(size_t num_vertices,
                   const graphlab_options& opts);


    void set_num_vertices(const lvid_type numv);

    void schedule(const lvid_type vid, double priority = 1 /* ignored */ );

    /** Get the next element in the queue */
    sched_status::status_enum get_next(const size_t cpuid,
                                       lvid_type& ret_vid);


    bool empty();

    static void print_options_help(std::ostream& out) {
      out << "\t multi = [number of queues per thread. Default = 3].\n";
    }


  }; 


} // end of namespace graphlab

#endif

