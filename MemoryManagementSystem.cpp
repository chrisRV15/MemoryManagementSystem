#include <iostream>
#include <unordered_map>
#include <list>
#include <stdexcept>
#include <vector>
using namespace std;

class Page {
public:
    int id;
    string data;

    //Constructor for the Page class
    Page(int id, string data) : id(id), data(data) {}
};

class Memory {
public:
    int size;   //Size of memory (number of frames)
    vector<Page*> frames;   //Vector of frames (can hold pages)
    unordered_map<int, list<int>::iterator> page_table; //Page table maps page ids to positions in the LRU list
    list<int> lru; //List to implement LRU
    unordered_map<int, bool> dirty_bit; //Tracks wheter a page has been modified
    int swap_space_size;
    unordered_map<int, Page*> swap_space;


    //Constructor
    Memory(int size, int swap_space_size) : size(size), swap_space_size(swap_space_size) {
        frames.resize(size, nullptr); //Initialize all frames asa nullptr
    }


    void load_page(Page* page) {

        //If memory is full, evict the least recently used page
        if (page_table.size() == size) {
            evict_page();
        }
        
        //Load the page into the first available frame
        for (int i = 0; i < size; i++) {
            if (frames[i] == nullptr) {
                frames[i] = page;
                break;
            }
        }

        //Add the page to the front of the LRU list
        lru.push_front(page->id);
        //Update the page table
        page_table[page->id] = lru.begin();
        dirty_bit[page->id] = false; //Dirty bit to false for newly loaded page
    }


    //Method to read data from a page in memory
    string read_memory(int page_id) {
        if (page_table.find(page_id) != page_table.end()) {
            //Move the accessed page to the front of the LRU list
            lru.erase(page_table[page_id]);
            lru.push_front(page_id);
            page_table[page_id] = lru.begin();

            //Return the data of the page
            for (Page* page : frames) {
                if (page != nullptr && page->id == page_id) {
                    return page->data;
                }
            }
        }
        else {
            handle_page_fault(page_id);
            return swap_space[page_id]->data;
        }
        // If the page is not in memory, throw an exception
        throw runtime_error("Page not found in memory");
    }

    void write_memory(int page_id, string new_data) {
        if (page_table.find(page_id) != page_table.end()) {
            //Move the accessed page to the front of the LRU list
            lru.erase(page_table[page_id]);
            lru.push_front(page_id);
            page_table[page_id] = lru.begin();

            //Modify the data of the page
            for (Page* page : frames) {
                if (page != nullptr && page->id == page_id) {
                    page->data = new_data;
                    dirty_bit[page_id] = true; //Dirty bit to true since page is modified
                    return;
                }
            }
        }
        else {
            handle_page_fault(page_id);
            swap_space[page_id]->data = new_data;
            dirty_bit[page_id] = true; //Dirty bit to true for newly loaded page
        }
    }

    //Method to free a page from memory
    void free_memory(int page_id) {
        if (page_table.find(page_id) != page_table.end()) {
            //Remove the page from the LRU list and the page table
            lru.erase(page_table[page_id]);
            page_table.erase(page_id);
            
            //Free the frame holding the page
            for (int i = 0; i < size; ++i) {
                if (frames[i] != nullptr && frames[i]->id == page_id) {
                    frames[i] = nullptr;
                    break;
                }
            }
        }
        else {
            //if the page is not in memory, throw an exception
            throw runtime_error("Page not found in memory");
        }
    }
private:
    void evict_page() {
        //LRU page replacement
        int lru_page_id = lru.back(); //Get the least recently used page
        lru.pop_back(); //Remove it from the LRU list

        //
        if (dirty_bit[lru_page_id]) {
            for (Page* page : frames) {
                if (page != nullptr && page->id == lru_page_id) {
                    swap_space[lru_page_id] = page;
                    dirty_bit[lru_page_id] = false;
                    break;
                }
            }
        }
        //Remove the page from the page table and free its frame
        page_table.erase(lru_page_id);
        for (int i = 0; i < size; ++i) {
            if (frames[i] != nullptr && frames[i]->id == lru_page_id) {
                frames[i] = nullptr;
                break;
            }
        }
    }

    void handle_page_fault(int page_id) {
        if (swap_space.find(page_id) != swap_space.end()) {
            //Page is in swap space, bring it back into memor
            if (page_table.size() == size) {
                evict_page(); // If no free frames, evict a page
            }

            //Load the page into the first available frame
            for (int i = 0; i < size; ++i) {
                if (frames[i] == nullptr) {
                    frames[i] = swap_space[page_id];
                    break;
                }
            }

            //Add the page to the front of the LRU list
            lru.push_front(page_id);
            //Update the page table
            page_table[page_id] = lru.begin();
            swap_space.erase(page_id);
        }
        else {
            throw runtime_error("Page not found in memory or swap space");
        }
    }
};


int main()
{
    Memory memory(3,2);   //Declare the frames and pages

    //Create some page objects
    Page* page1 = new Page(1, "Data1");
    Page* page2 = new Page(2, "Data2");
    Page* page3 = new Page(3, "Data3");
    Page* page4 = new Page(4, "Data4");

    //load the pages into memory
    memory.load_page(page1);
    memory.load_page(page2);
    memory.load_page(page3);


    cout << "Page 1 data: " << memory.read_memory(1) << endl;
    cout << "Page 2 data: " << memory.read_memory(2) << endl;
    cout << "Page 3 data: " << memory.read_memory(3) << endl;

    //Write to memory
    memory.write_memory(1, "Modified data1");

    //Read fro meory after modification
    cout << "Page 1 data: " << memory.read_memory(1) << endl;

    memory.load_page(page4);

    //Test
    try {
        cout << "Page 2 data: " << memory.read_memory(2) << endl;
    }
    catch (const runtime_error& e) {
        cout << e.what() << endl; 
    }

    memory.free_memory(1);
    //Test
    try {
        cout << "Page 1 data: " << memory.read_memory(1) << endl;
    }
    catch (const runtime_error& e) {
        cout << e.what() << endl;
    }

    return 0;
}

