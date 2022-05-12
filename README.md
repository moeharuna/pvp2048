# pvp2048
pvp2048 is a 1vs1 turn based multiplayer game based on 2048

## Gameplay
Game is basiclly 2048 but when one player did the swipe second player gets a turn.  
Each player has his own score and player get points for merge on his turn.  
Player who has most points when a game ends wins.  
Control the game by pressing arrow keys and maybe add some mouse only option like dragging or buttons.  

## Social interactions
- Both ability to play in guest mode and very basic registration.  
- There should be ability to play with friends and that means game invites
- Game should have matchmaking based on players elo.  
- Chat with opponent
- Maybe add some divisions on top of elo in ranked to create goal for players to reach. 

In terms of social we should aim for basic 1v1 first and build on top of this. Implementation in the order of complexity:
1. Invite / code based match
2. Random matching for guests
3. Elo matching (requires authentication and account management)
4. Segmented elo matching (e.g. different game modes have different queues)
5. Friend list is most likely an overkill unless we take one from socials (e.g. login through Steam, FB)


