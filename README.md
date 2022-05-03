# pvp2048
pvp2048 is a 1vs1 turn based multiplayer game based on 2048

## Gameplay
Game is basiclly 2048 but when one player did the swipe second player gets a turn.  
Each player has his own score and player get points for merge on his turn.  
Player who has most points when a game ends wins.  
Control the game by pressing arrow keys and maybe add some mouse only option like dragging or buttons.  

## Social interactions
- Both ability to play in guest mode and very basic registration.  
- There should be ability to play with friends and that means game invites(but friend list is probably overkill)  
- Game should have matchmaking based on players elo.  
- Chat with opponent(maybe dms?)
- Maybe add some divisions on top of elo in ranked to create goal for players to reach. 

In terms of social we should aim for basic 1v1 first and build on top of this. Implementation in the order of complexity:
1. Invite / code based match
2. Random matching for guests
3. Elo matching (requires authentication and account management)
4. Segmented elo matching (e.g. different game modes have different queues)
5. Friend list is most likely an overkill unless we take one from socials (e.g. login through Steam, FB)

## Some questions
- Does player who does first turn have advantage?
  - Yes, but the edge is hard to predict at this point. We can balance this with increasing play field size
- Should game spwan tiles completly random or have some sort of pattern?(its could be useful to discourage some behaivor or to make game more skill-based or to implemet comeback mechanic)
  - Let's start with default 2048 spawning mechanism

## Technical stuff
- We should find a way to manage dependencies early. Dependency management is always big pain in the ass in C++.
  - Any good recommendations?
    - Conan looks fine. Or we can download from pacman/apt. Or we could just copy-paste it in build tree.
- CorocachDB for Db.
- Docker
  - For local environment we will only use docker to run services - databases, etc.
  - Do we want to run application dockerized in production?
    - Im not sure.
- Should we use wasm for frontened or just plain js/ts?
  - I don't want to go wasm route since it's additional stack element that we are not familiar with. We can reconsider this later. gRPC+gRPC-web should be pretty safe bet for the start
