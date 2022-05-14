/*
     This file contains variables that the user can customize to their specifications
     such ad the nodeNumber and the mesh name and mesh password. 
     nodeNumber needs to be unique for each device
     Devies should be placed sequentially in the field so as to allow
     proper operation and sequential lighting
     
*/
int myNodeID = 1; // then 2 then 3........

//int receivedID = 0; //will hold the id received via the mesh broadcast

#define   MESH_PREFIX     "MESHNAME" //change for different or separate network
#define   MESH_PASSWORD   "MESHPASSWORD" //set the same for all nodes in one network
#define   MESH_PORT       5555

#define five_lamps 0 //change to 1 to go to 5 lamp mode

 #if five_lamps
#define xlamp 2
#else
#define xlamp 1
#endif

int calc(int theID){//pass in the id received from the mesh
     //this returns the state to which the lap should either be on or off
     int rangeMax = (theID + xlamp);
     int rangeMin = (theID - xlamp);
     rangeMin = (rangeMin<0)? 0 :rangeMin;
     
     if(myNodeID>=rangeMin && myNodeID<=rangeMax){
      return 1;
      }
      else{
          return 0;
        }
  }
