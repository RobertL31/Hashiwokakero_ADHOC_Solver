# Hashiwokakero_Services
Modules implemented for the Hashiwokakero resolution project

# Compiling the project 
g++ -std=c++17 service.cpp Island.cpp HashiGrid.cpp HashiState.cpp -lpistache -lstdc++fs -o solver

# Using the solver
You can either give a .has file (structure can be found is Hashi_Puzzles directory, they are specific csv files).
You should call the solver with ./solver filename

Also, the solver can be deployed as a web service, listening on localhost:50003. The deploy the solver that way, you should just use ./solver
The REST API of the solver, once deployed as a service, is developped with Pistache. Its implementation has not been a priority, and can be slightly improved.

```json
{
    "grid" : {
        "dimension": <int>,
        "description": [<int>, <int>, ...]
    }
}
```

The dimension field must contain a positive integer greater than 1, and the description field must contain a flat description of the grid, starting from top-left, of size $dimension * dimension$.

## Example

```json
[0,1,0,0]
[0,2,0,3]
[0,0,1,0]
[1,0,4,4]
```

would become

```json
{
    "grid" : {
        "dimension": 4,
        "description": [0,1,0,0,0,2,0,3,0,0,1,0,1,0,4,4]
    }
}

```