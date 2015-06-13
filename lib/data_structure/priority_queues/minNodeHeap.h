/******************************************************************************
 * minNodeHeap.h 
 *
 * Source of KaDraw -- Karlsruhe Graph Drawing 
 ******************************************************************************
 * Copyright (C) 2015 Christian Schulz <christian.schulz@kit.edu>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/

#ifndef MIN_NODE_HEAP_39CK1B8I
#define MIN_NODE_HEAP_39CK1B8I

#include <limits>
#include <vector>
#include <unordered_map>
#include <execinfo.h>

#include "data_structure/priority_queues/priority_queue_interface.h"
             
typedef int Key;

template < typename Data >
class QElement {
        public:
                QElement( Data data, Key key, int index ) : m_data(data), m_key (key), m_index(index) {}; 
                virtual ~QElement() {};

                Data & get_data() { 
                        return m_data; 
                }

                void set_data(Data & data) { 
                        m_data = data;
                }

                Key get_key() {
                        return m_key;
                }

                void set_key(Key key) {
                        m_key = key;
                }

                int get_index() {
                        return m_index;
                }

                void set_index(int index) {
                        m_index = index;
                }

        private:
                Data m_data;
                Key  m_key;
                int  m_index; // the index of the element in the heap

};

class minNodeHeap {
        public:
                              
                struct Data {
                        NodeID node;
                        Data( NodeID node ) : node(node) {};
                };

                typedef QElement<Data> PQElement;
                
                minNodeHeap() {};
                virtual ~minNodeHeap() {};

                NodeID size();  
                bool empty();

                bool contains(NodeID node);
                void insert(NodeID id, double key); 

                NodeID deleteMin();
                void deleteNode(NodeID node);
                NodeID minElement();
                double minValue();

                void decreaseKey(NodeID node, double key);
                void increaseKey(NodeID node, double key);
                void changeKey(NodeID node, double key); 
                double getKey(NodeID node); 

        private:
                std::vector< PQElement >                m_elements;      // elements that contain the data
                std::unordered_map<NodeID, int>         m_element_index; // stores index of the node in the m_elements array
                std::vector< std::pair<Key, int> >      m_heap;          // key and index in elements (pointer)

                void siftUp( int pos );
                void siftDown( int pos ); 

};



inline double minNodeHeap::minValue() {
        return m_heap[0].first;
};

inline NodeID minNodeHeap::minElement() {
        return m_elements[m_heap[0].second].get_data().node;
};

inline void minNodeHeap::siftDown( int pos ) {

        int curKey   = m_heap[pos].first;
        int lhsChild = 2*pos+1;
        int rhsChild = 2*pos+2;
        if( rhsChild < (int) m_heap.size() ) {

                int lhsKey = m_heap[lhsChild].first;
                int rhsKey = m_heap[rhsChild].first;

                if( curKey < lhsKey && curKey < rhsKey ) {
                        return; // we are done
                } else {
                        //exchange with the smaller one (minHeap)
                        int swap_pos = lhsKey < rhsKey ? lhsChild : rhsChild;
                        std::swap( m_heap[pos], m_heap[swap_pos]);

                        int element_pos = m_heap[pos].second;
                        m_elements[element_pos].set_index(pos);

                        element_pos = m_heap[swap_pos].second;
                        m_elements[element_pos].set_index(swap_pos);

                        siftDown(swap_pos);
                        return;
                } 

        } else if ( lhsChild < (int)m_heap.size()) {
                if( m_heap[pos].first > m_heap[lhsChild].first) {
                        std::swap( m_heap[pos], m_heap[lhsChild]);

                        int element_pos = m_heap[pos].second;
                        m_elements[element_pos].set_index(pos);

                        element_pos = m_heap[lhsChild].second;
                        m_elements[element_pos].set_index(lhsChild);

                        siftDown(lhsChild);
                        return;
                } else {
                        return; // we are done
                }
        }
}

inline void minNodeHeap::siftUp( int pos ) {
            if( pos > 0 ) {
                    int parentPos = (int)(pos-1)/2;
                    if(  m_heap[parentPos].first > m_heap[pos].first) {
                            //heap condition not fulfulled
                            std::swap(m_heap[parentPos], m_heap[pos]);

                            int element_pos = m_heap[pos].second;
                            m_elements[element_pos].set_index(pos);

                            // update the heap index in the element
                            element_pos = m_heap[parentPos].second;
                            m_elements[element_pos].set_index(parentPos);

                            siftUp( parentPos );
                    }

            }
}

inline NodeID minNodeHeap::size() {
        return m_heap.size();  
}

inline bool minNodeHeap::empty( ) {
        return m_heap.empty();        
}

inline void minNodeHeap::insert(NodeID node, double key) {
        if( m_element_index.find(node) == m_element_index.end() ) {
                int element_index =  m_elements.size();
                int heap_size     =  m_heap.size();

                m_elements.push_back( PQElement( Data(node), key, heap_size) );
                m_heap.push_back( std::pair< Key, int>(key, element_index) );
                m_element_index[node] = element_index;
                siftUp( heap_size );
        } 
}

inline void minNodeHeap::deleteNode(NodeID node) {
        int element_index = m_element_index[node]; 
        int heap_index    = m_elements[element_index].get_index();

        m_element_index.erase(node);

        std::swap( m_heap[heap_index], m_heap[m_heap.size() - 1]);
        //update the position of its element in the element array
        m_elements[m_heap[heap_index].second].set_index(heap_index);

        // we dont want holes in the elements array -- delete the deleted element from the array
        if(element_index != (int)(m_elements.size() - 1)) {
                std::swap( m_elements[element_index], m_elements[m_elements.size() - 1]);
                m_heap[ m_elements[element_index].get_index() ].second = element_index;
                int cnode              = m_elements[element_index].get_data().node;
                m_element_index[cnode] = element_index;
        }

        m_elements.pop_back();
        m_heap.pop_back();

        if( m_heap.size() > 1 && heap_index < (int)m_heap.size() ) {
                //fix the min heap property
                siftDown(heap_index);
                siftUp(heap_index);
        }

};

inline NodeID minNodeHeap::deleteMin() {
        if( m_heap.size() > 0) {
                int element_index = m_heap[0].second;
                int node = m_elements[element_index].get_data().node;
                m_element_index.erase(node);

                m_heap[0] = m_heap[m_heap.size() - 1];
                //update the position of its element in the element array
                m_elements[m_heap[0].second].set_index(0);

                // we dont want holes in the elements array -- delete the deleted element from the array
                if(element_index != (int)(m_elements.size() - 1)) {
                        m_elements[element_index] = m_elements[m_elements.size() - 1];
                        m_heap[ m_elements[element_index].get_index() ].second = element_index;
                        int cnode              = m_elements[element_index].get_data().node;
                        m_element_index[cnode] = element_index;
                }

                m_elements.pop_back();
                m_heap.pop_back();

                if( m_heap.size() > 1) {
                        //fix the heap property
                        siftDown(0);
                }

                return node;
        } 

        return -1;
}

inline void minNodeHeap::changeKey(NodeID node, double key) {
        double old_key = m_heap[m_elements[m_element_index[node]].get_index()].first; 
        if( old_key > key) {
                decreaseKey(node, key);
        } else if ( old_key < key) {
                increaseKey(node, key);
        }
};

inline void minNodeHeap::decreaseKey(NodeID node, double key) {
        ASSERT_TRUE(m_element_index.find(node) != m_element_index.end());
        int queue_idx = m_element_index[node]; 
        int heap_idx  = m_elements[queue_idx].get_index();
        m_elements[queue_idx].set_key(key);
        m_heap[heap_idx].first = key;
        siftUp(heap_idx);
}

inline void minNodeHeap::increaseKey(NodeID node, double key) {
        ASSERT_TRUE(m_element_index.find(node) != m_element_index.end());
        int queue_idx = m_element_index[node]; 
        int heap_idx  = m_elements[queue_idx].get_index();
        m_elements[queue_idx].set_key(key);
        m_heap[heap_idx].first = key;
        siftDown(heap_idx);
}

inline double minNodeHeap::getKey(NodeID node) {
        return m_heap[m_elements[m_element_index[node]].get_index()].first; 
};


inline bool minNodeHeap::contains(NodeID node) {
       return m_element_index.find(node) != m_element_index.end(); 
}

#endif
