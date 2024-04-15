#include <iostream>
#include <unordered_map>
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
    unordered_map<int, int> page_table; //Page table maps page ids to frame numbers

    //Constructor
    Memory(int size) : size(size) {
        frames.resize(size, nullptr); //Initialize all frames asa nullptr
    }


    void load_page(Page* page) {
        
        for (int i = 0; i < size; i++) {
            if (frames[i] == nullptr) {
                //load the page into the frame
                frames[i] = page;
                //Update the page table
                page_table[page->id] = i;
                return;
            }
        }
        //if all frames are full, throw an exception
        throw runtime_error("Memory is full");
    }


    //Method to read data from a page in memory
    string read_memory(int page_id) {
        //
        if (page_table.find(page_id) != page_table.end()) {
            //Get the frame number from the page table
            int frame_number = page_table[page_id];
            //Return the data of the page
            return frames[frame_number]->data;
        }
        else {
            // If the page is not in memory, throw an exception
            throw runtime_error("Page not found in memory");
        }
    }

    //Method to free a page from memory
    void free_memory(int page_id) {
        //
        if (page_table.find(page_id) != page_table.end()) {
            //Get the frame number from the page table
            int frame_number = page_table[page_id];
            //Free the frame
            frames[frame_number] = nullptr;
            //Remove the page from the page table
            page_table.erase(page_id);
        }
        else {
            //if the page is not in memory, throw an exception
            throw runtime_error("Page not found in memory");
        }
    }
};


int main()
{
    Memory memory(3);   //Declare the frames

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


    memory.free_memory(1);
    memory.free_memory(2);
    memory.free_memory(3);


    delete page1;
    delete page2;
    delete page3;

    return 0;
}

