//LED instantiations
const int ledPin13 = 13;
const int ledPin12 = 12;
const int ledPin11 = 11;
const int ledPin10 = 10;
const int ledPin9 = 9;
const int ledPin8 = 8;
const int ledPin7 = 7;
const int ledPin6 = 6;
const int ledPin5 = 5;
const int ledPin4 = 4;
const int ledPin3 = 3;
int ledBlinkState = HIGH;
unsigned long prevMillis = 0;
const long blinkInterval = 50;

int ledPins[3][3] = {{13, 12, 11},  // holds layout of LEDs
                     {10, 9, 8 },
                     {7, 6, 5}};
int grid[3][3];                     // stores the state of the board
int selectedr = 1;                  // row position of the "cursor"         
int selectedc = 1;                  // columb position of the "cursor"
int currTurn;                       // variable to store the current turn 
                                    // (0 or 1 for x or y/blinky or solid)
int rounds = 0;                     // stores the current round of the game 
                                    // (starting at round 1)
int presses = 0;                    // stores the amount of button presses
                                    // used for incrementing rounds at the right time
int whoFirst;                       // holds who goes first
                                    // 1 for human, 0 for computer

//Button instantiation
const int buttonPin = 2;
int buttonState = 0;

//Thumbstick instantiations
const int switch_pin = 1;
const int x_val = 0;
const int y_val = 1;

void setup() {
  //Initial setup for thumbstick
  pinMode(switch_pin, INPUT);
  digitalWrite(switch_pin, HIGH);
  Serial.begin(115200);
  
  //Setup for selection button
  pinMode(buttonPin,INPUT);
  //Setup for LED grid
  
  for(int i = 0; i < 3; i++){
    for( int j = 0; j < 3; j++){
      pinMode(ledPins[i][j], OUTPUT);
    }
  }
  
  pinMode(ledPin4, OUTPUT);
  pinMode(ledPin3, OUTPUT);
}

// function to pick who goes first
int pick(){
    int choice;
    randomSeed(analogRead(0));
    choice = random(2);
    return choice;
}// end function pick()

// utility function to flip a value
int flip(int x){
    if(x == 0)
        return 1;
    else
        return 0;
}// end function flip()

// utility function to copy the contents of one grid
// into another grid
void copyGrid(int grid[][3], int temp[][3]){
    for(int r = 0; r < 3; r++){
        for(int c = 0; c < 3; c++){
            temp[r][c] = grid[r][c]; 
        }
    }
}// end function copyGrid()

// utility function to check if someone won
int isWin(int grid[][3]){
    int value;
    if((grid[0][0] == 1 || grid[0][0] == 0) && grid[0][0] == grid[1][1] && grid[1][1] == grid [2][2])
        // check diagonal from top left to bottom right
        return grid[0][0];
    if((grid[0][0] == 1 || grid[0][0] == 0) && grid[0][2] == grid[1][1] && grid[1][1] == grid [2][0])
        // check diagonal from top right to bottom left
        return grid[0][2];
    for(int c = 0; c < 3; c++){
        // checks the vertical cases
        if(grid[0][c] != -1){
            value = grid[0][c];
            bool temp = true;
            for(int i = 1; i <= 2; i++){
                if(grid[i][c] != value)
                    temp = false;
                if(i == 2 && temp == true)
                    return grid[0][c]; 
                    // return who won           
            }
        }
    }
    for(int r = 0; r < 3; r++){
        // checks the horizontal cases
        if(grid[r][0] != -1){
            value = grid[r][0];
            bool temp = true;
            for(int i = 1; i <= 2; i++){
                if(grid[r][i] != value)
                    temp = false;
                if(i == 2 && temp == true)
                    return grid[r][0]; 
                    // return who won           
            }
        }
    }
    return -1;
    // return -1 if no one won
}// end function isWin()

// utility function to check if the game is over
bool isOver(int grid[][3]){
    if(isWin(grid) != -1)
        // if someone won
        return true;
        // the game is over
    bool over = true;
    // the game is over
    // unless an empty spot is found
    for(int r = 0; r < 3; r++){
        for(int c = 0; c < 3; c++){
            if(grid[r][c] == -1)
                over = false;
        }
    }
    return over;
}
// end function isOver()

// function to display the who won
// or if it was a tie
void results(int grid[][3]){
  if ( (isWin(grid) == 0 && whoFirst == 1) || (isWin(grid) == 1 && whoFirst == 0) ){
    // human won                                                  
    // light up and blink the human light                    
    digitalWrite(ledPin3, HIGH);
    // delay 5 seconds                                        
    delay(3000);
    digitalWrite(ledPin3, LOW);
  }
  if ( (isWin(grid) == 0 && whoFirst == 0) || (isWin(grid) == 1 && whoFirst == 1) ){
    // computer won
    // light up and blink the computer light                    
    digitalWrite(ledPin4, HIGH);
    // delay 5 seconds                                  
    delay(3000);
    digitalWrite(ledPin4, LOW);
  }
  if (isWin(grid) == -1){
    // draw
    // light up and blink the both lights                 
    digitalWrite(ledPin4, HIGH);
    digitalWrite(ledPin3, HIGH);
    // delay 5 seconds                                        
    delay(3000);
    digitalWrite(ledPin4, LOW);
    digitalWrite(ledPin3, LOW);
  }
}// end function results()

// function to wipe the grid, and start a new game
void newGame( int grid[][3] ){
  for(int r = 0; r < 3; r++){
    for(int c = 0; c < 3; c++){
      grid[r][c] = -1;
      // fills grid with empty spots
    }
  }
  whoFirst = pick();
  // pick who goes first
  //If the player is first, light Green LED
  if(whoFirst == 1){
    digitalWrite(ledPin4, HIGH);
    delay(2000);
    digitalWrite(ledPin4, LOW);
  }
  //If the computer is first, light Red LED
  else if(whoFirst == 0){
    digitalWrite(ledPin3, HIGH);
    delay(2000);
    digitalWrite(ledPin3, LOW);
  }
  
  currTurn = 1;
  // set current turn to solid
  // (solid always goes first)
  // (blinky always goes second)
  rounds = 1;
  // set game to first round
  presses = 0;
  // refresh press counter
}
// end function newGame

// function that moves the "cursor"
void thumbMove(int x, int y){
  if(( 375 < x < 700) && (y == 0)){
    if(selectedr > 0){
      // if not in the top and the player moves up
      selectedr -= 1;
      // go up
    }
  }
  else if(( 375 < x < 700) && (y == 1023)){
    if(selectedr < 2){
      // if not in the bottom and the player moves down
      selectedr += 1;
      // go down
    }
  } 
  // if not in the first column and the player moves to left
  else if((x == 0) && (375 < y < 700)){
    if(selectedc > 0){
      selectedc -= 1;
      // go left
    }
  } 
  // if it's not in the first column and the player moves to left
  else if((x == 1023) && (375 < y < 511)){
     if(selectedc < 2){
      selectedc += 1;
      // go right
    }
  } 
}// end function thumbMove

// puts the choice on the grid, and tracks turns
void makeChoice(int x,int r,int c){
  grid[r][c] = x;
  digitalWrite(ledPins[r][c], 125);
  currTurn = flip(currTurn);
  presses++;
  if(presses%2 == 0){
    rounds++;
  }
}// end function makeChoice

void computer(int grid[][3], int turn, int moves){
    if(grid[1][1] == -1){
        // go for the middle if it's open
        makeChoice( turn, 1, 1);
        return;
    }
    else{
        int sub[3][3];
        copyGrid(grid, sub);
        // create a temporary grid and fill the board into it
        // this first part essentially searches for a winning move
        // and then takes it
        for(int r = 0; r < 3; r++){
            for(int c = 0; c < 3; c++){
                // go through each spot
                if(sub[r][c] == -1)
                    // if the spot in the copy is empty
                    sub[r][c] = turn;
                    // fill it
                if(isWin(sub) != -1){
                    // if someone won
                    makeChoice( turn, r, c );
                    // put that move into the actual board
                    return;
                }else{
                    copyGrid(grid, sub);
                    // copy the board into the copy again
                }
            }
        }
        copyGrid(grid, sub);
        // copy the current board into a temp
        // if the opponent is about to win,
        // block that move
        for(int r = 0; r < 3; r++){
            for(int c = 0; c < 3; c++){
                if(sub[r][c] == -1)
                    sub[r][c] = flip(turn);
                if(isWin(sub) != -1){
                    makeChoice( turn, r, c );
                    return;
                }else{
                    copyGrid(grid, sub);
                }
            }
        }
    }
    // should choose the opposite side of the opponent's move
    if(moves == 2 && turn == 1){
        int x, y;  
        for(int r = 0; r < 3; r++){
            for(int c = 0; c < 3; c++){
                if(grid[r][c] == flip(turn)){
                    // find the first spot taken by the opponent
                    x = r;
                    y = c;
                }
            }
        }
        if(x == 0)
            // if opponent is in row 1, we will go in row 3
            x += 2;
        else if (x == 1)
            // if oppenent is in row 2, we will go in row 1
            x -= 1;
        else
            // if opponent is in row 3, we will go in row 1
            x -= 2;
        if(y == 0) 
            y += 2;
        else if (y == 1)
            y -= 1;
        else
            y -= 2;
        if(grid[x][y] == -1){
            makeChoice( turn, x, y );
            return;
        }
    }
    if(moves == 1 && turn == 0){
        // if the computer is going second, go on the top right corner
        // if the middle isn't open
        if(grid[0][0] == -1){
            makeChoice( turn, 0, 0 );
            return;
        }
        // this should never be needed,
        // but just in case.
        else if(grid[0][2] == -1){
            makeChoice( turn, 0, 2 );
            return;
        }
    }

    if(moves == 2 && turn == 0){
        // if the computer is going second
        // then the second move must also be on the corner
        // or else it is easy for the human to win
        if(grid[0][0] == -1){
            makeChoice( turn, 0, 0 );
            return;
        }
        else if(grid[0][2] == -1){
            makeChoice( turn, 0, 2 );
            return;
        }
        else if(grid[2][2] == -1){
            makeChoice( turn, 2, 2 );
            return;
        }
        else{
            makeChoice( turn, 2, 0 );
            return;
        }
    }
    
    int count = 0;
    
    // count the empty spots         
    for(int r = 0; r < 3; r++){
        for(int c = 0; c < 3; c++){
            if(grid[r][c] == -1){
                count++;
            } 
        }
    }// finished counting

    
    int i = 0;
    // counter
    int posr[count];
    // row index
    int posc[count];
    // column index
     for(int r = 0; r < 3; r++){
        for(int c = 0; c < 3; c++){
            if(grid[r][c] == -1){
               posr[i] = r;
               // index of available rows
               posc[i] = c;
               // index of available columns
               i++;  
            } 
        }
    }
    // picks a random space from the index of available spots
    int choice;
    randomSeed(analogRead(0));
    choice = random(count);
    if(grid[posr[choice]][posc[choice]] == -1){
        makeChoice( turn, posr[choice], posc[choice] );
        // pick a random free space (makes sure it is in fact free)
        return; 
    }
    // last option: pick the first empty spot
    for(int r = 0; r < 3; r++){
        for(int c = 0; c < 3; c++){
            if(grid[r][c] == -1){
                makeChoice( turn, r, c );
                return;
            } 
        }
    } 
     
}
// end ai

// main loop
void loop() {
  unsigned long currentMillis = millis();
  // track the passing milliseconds
  if((currentMillis - prevMillis) >= blinkInterval){
    // use the passing milliseconds to turn the blinking lights on and off
    prevMillis = currentMillis;
    if(ledBlinkState == LOW){
      ledBlinkState = HIGH;
    }
    else{
      ledBlinkState = LOW;
    }
  } 
  buttonState = digitalRead(buttonPin);
  // setup the button

  if(rounds == 0){
    // when the game is started up, call newGame
    newGame(grid);
  }

  // cursor light up 
  digitalWrite(ledPins[selectedr][selectedc], HIGH);
  // control the non cursor lights
  for(int i = 0; i < 3; i++){
    for(int j = 0; j < 3; j++){
      if((i != selectedr || j != selectedc) && (grid[i][j] == -1)){
        digitalWrite(ledPins[i][j], LOW);
      }
      if(grid[i][j] == 0){
        digitalWrite(ledPins[i][j], ledBlinkState);
      }
    }
  }
  
  // thumbstick control
  thumbMove(analogRead(x_val), analogRead(y_val));
  delay(150);

  // if the computer went first and it's his turn
  if(whoFirst == 0 && currTurn == 1){
    // do the computer's turn
    computer(grid, currTurn, rounds);
  }
  else if(whoFirst == 0 && currTurn == 0 && buttonState == HIGH){
    // if the computer went first and he already went
    // wait for a button press then do the human turn
    delay(100);
    if (grid[selectedr][selectedc] == -1){
      makeChoice(currTurn, selectedr, selectedc);
      delay(100);
    }
  }

  // if the human went first and the button is pressed
  else if (buttonState == HIGH && whoFirst == 1){
    delay(100);
    // advances a round with human first
    if (grid[selectedr][selectedc] == -1){
      makeChoice(currTurn, selectedr, selectedc);
      delay(100);
    }
  }
  // if the human already went then do the computer turn
  else if(whoFirst == 1 && currTurn == 0){
    computer(grid, currTurn, rounds);
  }

// if the game is over,
// display the results (win/lose/draw)
// and start a new game.
  if (isOver(grid)){
    results( grid );
    delay(1000);
    newGame(grid);
  }
}
