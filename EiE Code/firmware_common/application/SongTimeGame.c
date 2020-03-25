/*game in C
first edition will likely need to change to debug for board
This is going to be a sound timer game
User will be required to try to time the beat
Hit the button at the same time as the beat 
Led indicators on for the beat time
*/

static void Game1(void)
{
    //should increase in difficulty
    //speed increases slowly
    //first gives about 5 button presses until
    //start
    //sound turns off eventually
    //leds turn off eventually
    static int FirstTime = 1;
    if (FirstTime = 1) //so that id doesn't display each loop
    {
        LCDMessage(LINE1_START_ADDR, "Welcome 2 Conductor");
        LCDMessage(LINE2_START_ADDR, "Start     "); //need to test out the spaces after
    }
    LCDMessage(LINE2_END_ADDR - 3, "QUIT"); //Will continue to be here

    static int GameLevel = 0;   //difficulty will increase
    static int startTime = 0;   //start timeing for the beats
    static int Display = 0; //will be 1 if a average time should be shown to the user
                            //will be count down from 11 to 6 (5) button presses to start
                            //will then count from 6 to 1 when user is being timed
    int forgivness = 300 - (GameLevel * 25); //In ms The closeness the user has to be to the beat divided by two to pass it
                    //this is an import to adjust the difficulty
    static int interval = 500; //in ms //the time between beats
                    //adjust this line sometimes don't know when yet
    static int averageTime = 0; //should change on the real button presses the user has
                    //build a equation later on for this when Display = 1
                    //should be displayed to the user

    if (GameLevel > 0)  //functions will activate/disable depending on level
    {
        //NEED TO:
            //reset the startTime and and average time

        if (startTime == 0)//sets all the values to start
        {
            Display = 11; // begins the count down
            startTime = G_u32SystemTime1ms; //sets the start time
        }

        if ((Display > 1) && ((G_u32SystemTime1ms - startTime) % interval <= 6))//preform the function for 6 ms
        {                               //may need to adjust the parameteres of the if
            //buzzer sounds for 6 ms    //or something //maybe change for each level

            //LED maybe the same time
        }

        if (Display > 1 && Display < 7 && (WasButtonPressed(BUTTON0) || WasButtonPressed(BUTTON1) || WasButtonPressed(BUTTON2)))    //average acumulation
        {
            ButtonAcknowledge(BUTTON0);
            ButtonAcknowledge(BUTTON1);
            ButtonAcknowledge(BUTTON2);

            averageTime = ((G_u32SystemTime1ms - startTime) % interval - 3); //the time difference I think //need to adjust to make the average


        }


        if (GameLevel < 5) //LED     //will need to make adjustments on the activation times    //may change this
        {
            //LEDs flash on at the press time should last the duration of the forgivness of the level
        }

        

    } //end game run "function"

    if ((Display > 1) && (Display < 7)) //disoplays the current average time
    {
        //turns off after 3 seconds
    }

    else if (WasButtonPressed(BUTTON0)) //start game option
    {
        LCDMessage(LINE1_START_ADDR, "                   "); //coverse the previous message
        ButtonAcknowledge(BUTTON0);
        GameLevel = 1;
        FirstTime = 0; //incase the user pressed the button during welcome message
    }
    if (WasButtonPressed(BUTTON3)) //Quit option should show final score (level made)
    {
        UserApp1_pfStateMachine = ChooseGame;
    }
    
    LCDMessage(LINE1_START_ADDR, " ");


}

