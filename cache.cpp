#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <cmath>

struct block{
  uint64_t tag; // The rest of the bits that aren't the address index or the set index
  uint64_t counter; // Last time it was used. How many ticks ago.
  bool dirty = false; //Says if its there or not
};
//-------------------------------------------------------------------------------------------------------
uint64_t getSetIndex(uint64_t addrNum, uint64_t setMask, uint64_t address){
  return ((address & setMask)>>addrNum);
}

uint64_t getTag(uint64_t addrNum, uint64_t setNum, uint64_t address){
  return (address>>(setNum+addrNum));
}

uint64_t findIndex(block* set, uint64_t tag, uint64_t blocksCount, uint64_t* missed){
  uint64_t lruIndex = 0;
  uint64_t index = 0;
  bool found = false;
  for(uint64_t i=0; i<blocksCount; ++i){
    if(!set[i].dirty){
      lruIndex = i;
      break;
    }
    if(set[i].tag == tag){
      index = i;
      found = true;
    }
    set[i].counter++;
    lruIndex = (set[lruIndex].counter < set[i].counter)? i:lruIndex;
  }
  if(found){
    return index;
  }
  else{
    (*missed)++;
    return lruIndex;
  }
}
//-----------------------------------------------------------------------------------
int main(int argc , char* argv[]){
//-------------------------------------------------------------------------------------------------------
//This is here because I am anal about how the beginning of my terminal display looks. Don't fucking judge me man.
  std::cout<<"\n";
//-------------------------------------------------------------------------------------------------------

//Variables
  uint64_t missed = 0;
  uint64_t blockIndex = 0;
  uint64_t setIndex = 0;
  uint64_t addrCount = 0;
  uint64_t setCount = 0;
  uint64_t blocksCount = 0;
  uint64_t setMask = 0;
  uint64_t tag;
//-------------------------------------------------------------------------------------------------------

//Reads in the first 3 arguments
  uint64_t setNum = stoul(std::string(argv[1]), nullptr, 0); //x
  uint64_t blocksNum = stoul(std::string(argv[2]), nullptr, 0); //y
  uint64_t addrNum = stoul(std::string(argv[3]), nullptr, 0); //z
  setMask = ~(~0ULL<<setNum)<<addrNum;
  blocksCount = (1ULL<<blocksNum);  //2​^y ​ is the number of blocks per set
  addrCount = (1ULL<<addrNum);   //2^z ​ is the number of addresses cached in each block
  setCount = (1ULL<<setNum);  //2​^x ​ is the number of direct-mapped sets
//-------------------------------------------------------------------------------------------------------

//Displays the arguments
  std::cout<<setNum<<"\n";
  std::cout<<blocksNum<<"\n";
  std::cout<<addrNum<<"\n\n";
  std::cout<<setCount<<"\n";
  std::cout<<blocksCount<<"\n";
  std::cout<<addrCount<<"\n\n";
//-------------------------------------------------------------------------------------------------------

//Creates vector of blocks as the set array
  std::vector<block> cache = std::vector<block>(setCount*blocksCount);

//-------------------------------------------------------------------------------------------------------

//Reads in and opens the file
  uint64_t number = 0;
  char *fileName = argv[4];
  std::string line;
  std::vector<uint64_t> file_array;
  std::ifstream file(fileName);
//-------------------------------------------------------------------------------------------------------

//Pushes file contents into a vector named file_array for manipulation
  while (std::getline(file, line))
  {
    std::stringstream ss(line);
    ss>>std::hex>>number;
    file_array.push_back(number);
  }
//-------------------------------------------------------------------------------------------------------

//Gets the length of the file array for display purposes
  uint64_t length = file_array.size();
//-------------------------------------------------------------------------------------------------------

//Meaty Stuff
for(const auto& line:file_array){
  setIndex = getSetIndex(addrNum, setMask, line);
  block* set = &cache[setIndex*blocksCount];
  tag = getTag(addrNum, setNum, line);
  blockIndex = findIndex(set, tag, blocksCount, &missed);
  block* block = set + blockIndex;
  if(block->tag != tag){
    block->tag = tag;
    block->dirty = true;
  }
  block->counter = 0;
}
//Displays results
  std::cout <<"missed "<<missed<<" / "<<length<< "\n\n";


}
