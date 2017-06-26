package com.babcock.spellingsquared;

import android.animation.ArgbEvaluator;
import android.animation.ValueAnimator;
import android.app.ProgressDialog;
import android.content.DialogInterface;
import android.content.SharedPreferences;
import android.content.res.AssetManager;
import android.graphics.Color;
import android.graphics.Point;
import android.graphics.Typeface;
import android.graphics.drawable.GradientDrawable;
import android.support.v7.app.AlertDialog;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.TypedValue;
import android.view.Display;
import android.view.Gravity;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.TextView;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.Date;
import java.util.Random;
import java.util.Scanner;

public class MainActivity extends AppCompatActivity {

    //widgets declaration
    Button SignOutButton, SingleMenuButton, MultiMenuButton, QuickMatchButton, ChallengeButton, ViewGamesButton, QuitButton, ResultsButton;
    Button NewPuzzleButton, ScrambleButton, HintButton, SolutionButton, TipsButton;
    com.google.android.gms.common.SignInButton SignInButton;
    LinearLayout MultiMenuView, Header, MenuView;
    RelativeLayout Gameboard, background, Screen;
    TextView TimerTextView;

    //global variables most of which are used for multiplayer functions
    private static ProgressDialog progress;
    private static AlertDialog mAlertDialog;
    public boolean isMultiplayerMode = false;
    private static ArrayList<SavedGame> savedList = new ArrayList<>();
    private static CurrentGame currentGame;
    private static String currId = "default";

    //global variables used for regular puzzle solving
    private static GradientDrawable lightGD, darkGD, yellowGD;
    private static Date startTime;
    private static SharedPreferences pref;
    private static Integer time = 0;
    //holds letters in 4x4 grid
    private static ArrayList<TextView> squareList = new ArrayList<>();
    //holds transparent views to flash yellow
    private static ArrayList<RelativeLayout> goldList = new ArrayList<>();
    //word string holds all puzzles in a file, others hold solution for puzzle and current state of board
    //puzzle is solved when solution string == board string
    private static String wordString = "", solutionString = "abutbareedgedeed", boardString = "gbederauetdedeba";
    private static ArrayList<Integer> firstList = new ArrayList<>();
    //holds eight words in solution
    private static ArrayList<String> solutionList = new ArrayList<>();
    //holds words user has already correctly guessed
    private static ArrayList<String> guessedList = new ArrayList<>();
    private static Integer firstPressed = -1, myYellow, myGreen, myBlue, finalYellow;
    //flags set in touch events
    private static Boolean twoFingers = false, isVertical = false, signInClicked = false, keepSettingBlue = true;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        pref = getSharedPreferences("AppData", MODE_PRIVATE);

        //prepare gradients
        setGradients();

        //load puzzles from a text file in Assets folder
        readFile();

        //assign widgets to XML counterparts
        setWidgets();

        //recover important data from previous session
        //eg - last puzzle and time spent on it
        getSavedData();
    }

    @Override
    protected void onStart() {
        super.onStart();

        //get a fresh start time
        startTime = new Date();

        //build UI
        setBoard();
    }

    @Override
    protected void onStop() {
        super.onStop();
    }

    @Override
    protected void onPause() {
        super.onPause();

        //increment time spent on current puzzle
        addTime();

        //save to Shared Preferences
        saveData();
    }

    //prepare dynamically built gradients for use in board
    private void setGradients() {
        myYellow = Color.parseColor("#ffffb3");
        finalYellow = Color.parseColor("#ffff66");
        myGreen = Color.parseColor("#80ff80");
        myBlue = Color.parseColor("#0073e6");
        int[] lightColors = {Color.parseColor("#cce6ff"),Color.parseColor("#ffffff")};
        int[] darkColors = {Color.parseColor("#66b3ff"),Color.parseColor("#99ccff")};
        int[] yellowColors = {Color.parseColor("#ffffe6"),Color.parseColor("#ffffff")};
        lightGD = new GradientDrawable(GradientDrawable.Orientation.TOP_BOTTOM, lightColors);
        lightGD.setCornerRadius(3);
        darkGD = new GradientDrawable(GradientDrawable.Orientation.TOP_BOTTOM, darkColors);
        darkGD.setCornerRadius(3);
        yellowGD = new GradientDrawable(GradientDrawable.Orientation.TOP_BOTTOM, yellowColors);
        yellowGD.setCornerRadius(3);
    }

    //picks a random text file and loads its puzzles
    private void readFile() {
        AssetManager am = getAssets();
        try
        {
            Random rand = new Random();
            int r = rand.nextInt(100);
            InputStream is = am.open("f" + String.valueOf(r) +".txt");
            wordString = new Scanner(is, "UTF-8").useDelimiter("\\A").next();
        }
        catch (IOException e) {}
    }

    //assigns widgets to views in XML layout
    //sets listeners
    private void setWidgets() {
        TimerTextView = (TextView)findViewById(R.id.TimerTV);

        SignInButton = (com.google.android.gms.common.SignInButton)findViewById(R.id.signInButton);
        SignOutButton = (Button)findViewById(R.id.SignOutButton);
        SingleMenuButton = (Button)findViewById(R.id.SingleMenuButton);
        MultiMenuButton = (Button)findViewById(R.id.MultiMenuButton);
        QuickMatchButton = (Button)findViewById(R.id.QuickMatchButton);
        ChallengeButton = (Button)findViewById(R.id.ChallengeButton);
        ViewGamesButton = (Button)findViewById(R.id.ViewGamesButton);
        QuitButton = (Button)findViewById(R.id.QuitButton);
        NewPuzzleButton = (Button)findViewById(R.id.NewPuzzleButton);
        ScrambleButton = (Button)findViewById(R.id.ScrambleButton);
        HintButton = (Button)findViewById(R.id.HintButton);
        SolutionButton = (Button)findViewById(R.id.SolutionButton);
        TipsButton = (Button)findViewById(R.id.TipsButton);

        MultiMenuView = (LinearLayout)findViewById(R.id.MultiMenuView);
        MenuView = (LinearLayout)findViewById(R.id.MenuView);
        Header = (LinearLayout)findViewById(R.id.Header);
        Gameboard = (RelativeLayout)findViewById(R.id.GameBoard);
        Screen = (RelativeLayout)findViewById(R.id.Screen);

        SignInButton.setOnClickListener(new com.google.android.gms.common.SignInButton.OnClickListener() {
            @Override
            public void onClick(View v) {
                //disabled in demo version
            }
        });

        SignOutButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //disabled in demo version
            }
        });

        MultiMenuButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(MultiMenuView.getVisibility() == View.VISIBLE) MultiMenuView.setVisibility(View.INVISIBLE);
                else MultiMenuView.setVisibility(View.VISIBLE);
            }
        });

        SingleMenuButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(MenuView.getVisibility() == View.VISIBLE) MenuView.setVisibility(View.INVISIBLE);
                else MenuView.setVisibility(View.VISIBLE);
            }
        });

        NewPuzzleButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                MenuView.setVisibility(View.INVISIBLE);
                resetPuzzle();
            }
        });

        ScrambleButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                MenuView.setVisibility(View.INVISIBLE);
                setBoardBlue();
                boardString = scrambleSolution(solutionString);
                for(int i = 0; i<16; i++) {
                    squareList.get(i).setText(boardString.substring(i, i+1));
                }
            }
        });

        HintButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                MenuView.setVisibility(View.INVISIBLE);
                showHint();
            }
        });

        SolutionButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                MenuView.setVisibility(View.INVISIBLE);
                showSolution();
            }
        });

        TipsButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                MenuView.setVisibility(View.INVISIBLE);
                showWarning("Tips","Arrange sixteen random letters to spell four words across and four words down as fast as you can\r\n\r\n" +
                        "Use one one finger to move a letter, and two fingers to move a word\r\n\r\n" +
                        "A word will flash yellow when placed in the correct position, and the whole board turns yellow upon reaching the solution\r\n\r\n" +
                "If you spell a word in the solution but not in correct position, the board will flash green - only one time per word\r\n\r\n" +
                "Use the Multiplayer Menu to challenge a friend or random opponent to a best of three competition. Find the same solutions in faster times to win!\r\n\r\n" +
                "The best way to open a multiplayer game is not through a notification. When you receive notice of your turn, simply open the app and proceed to View Your Games");
            }
        });

        ViewGamesButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //disabled in demo version
            }
        });

        ChallengeButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //disabled in demo version
            }
        });

        QuickMatchButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //disabled in demo version
            }
        });

        QuitButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //disabled in demo version
            }
        });
    }

    //if in multiplayer mode, this is not allowed
    //otherwise, shows the solution of the current puzzle in a dialog
    private void showSolution() {
        String sol = "Viewing the solution is not allowed during multiplayer games";
        if(!isMultiplayerMode) sol = solutionList.get(0) + "\r\n" + solutionList.get(1) + "\r\n" + solutionList.get(2) + "\r\n" + solutionList.get(3);
        AlertDialog.Builder dlgAlert = new AlertDialog.Builder(MainActivity.this);
        dlgAlert.setMessage(sol);
        dlgAlert.setTitle("Solution");
        dlgAlert.setPositiveButton("Got it!", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int whichButton) {
                dialog.cancel();
            }
        });
        dlgAlert.setCancelable(true);
        dlgAlert.create().show();
    }

    //picks one letter in each word to show user via dialog, other letters replaced by ?
    //hints not allowed in multiplayer mode
    private void showHint() {
        String hint = "";
        if(!isMultiplayerMode) {
            ArrayList<Integer> hList = new ArrayList<>();
            hList.add((solutionString.charAt(3) * 42) % 4);
            hList.add(((solutionString.charAt(5) * 42) % 4) + 4);
            hList.add(((solutionString.charAt(8) * 42) % 4) + 8);
            hList.add(((solutionString.charAt(13) * 42) % 4) + 12);
            for(int i = 0; i<16; i++) {
                if(hList.contains(i)) hint += solutionString.substring(i, i+1);
                else hint += "?";
                if((i+1)%4 == 0) hint += "\r\n";
            }
        }
        else hint = "Hints are not during multiplayer games";
        AlertDialog.Builder dlgAlert = new AlertDialog.Builder(MainActivity.this);
        dlgAlert.setMessage(hint);
        dlgAlert.setTitle("Hint");
        dlgAlert.setPositiveButton("Got it!", new DialogInterface.OnClickListener() {
            public void onClick(DialogInterface dialog, int whichButton) {
                dialog.cancel();
            }
        });
        dlgAlert.setCancelable(true);
        dlgAlert.create().show();
    }

    //clears any views in the board, builds UI again
    private void setBoard() {
        try{
            goldList.clear();
            squareList.clear();
            Gameboard.removeAllViews();
        }
        catch(Exception er) {}

        Display display = this.getWindowManager().getDefaultDisplay();
        Point size = new Point(); display.getSize(size);
        int width = (int) (size.x * 0.9);
        int sqWidth = width / 5;
        int border = width / 25;

        background = new RelativeLayout(this);
        RelativeLayout.LayoutParams bp = new RelativeLayout.LayoutParams(width, width);
        background.setLayoutParams(bp);
        background.setBackgroundColor(myBlue);
        RelativeLayout.LayoutParams gp = new RelativeLayout.LayoutParams(RelativeLayout.LayoutParams.WRAP_CONTENT, RelativeLayout.LayoutParams.WRAP_CONTENT);
        gp.topMargin = ((size.y - width)/2) + sqWidth/2;
        gp.leftMargin  = (size.x - width) / 2;
        Gameboard.setLayoutParams(gp);
        Gameboard.addView(background);

        //places transparent views behind row and columns
        //will later be flashed with yellow color to show a word is correctly placed
        int margin = 0;
        ArrayList<RelativeLayout> tempList = new ArrayList<>();
        for(int i = 0; i<4; i++) {
            RelativeLayout v = new RelativeLayout(this);
            RelativeLayout.LayoutParams p = new RelativeLayout.LayoutParams(width, (border * 2) + sqWidth);
            p.leftMargin = 0;
            p.topMargin = margin;
            v.setLayoutParams(p);
            v.setBackgroundColor(Color.TRANSPARENT);
            background.addView(v);
            goldList.add(v);
            RelativeLayout v2 = new RelativeLayout(this);
            RelativeLayout.LayoutParams p2 = new RelativeLayout.LayoutParams((border * 2) + sqWidth, width);
            p2.leftMargin = margin;
            p2.topMargin = 0;
            v2.setLayoutParams(p2);
            v2.setBackgroundColor(Color.TRANSPARENT);
            background.addView(v2);
            tempList.add(v2);
            margin += border + sqWidth;
        }
        goldList.addAll(tempList);

        //creates fonts from assets/fonts
        int x = border, y = border;
        Typeface candela = Typeface.createFromAsset(getAssets(), "fonts/CandelaBook.otf");
        Typeface serif = Typeface.create(Typeface.SANS_SERIF, Typeface.BOLD);

        //creates 4x4 grid for gameplay
        for(int i = 0; i<16; i++) {
            TextView square = new TextView(this);
            square.setTypeface(candela);
            square.setBackground(lightGD);
            RelativeLayout.LayoutParams params = new RelativeLayout.LayoutParams(sqWidth, sqWidth);
            params.leftMargin = x;
            params.topMargin = y;
            square.setLayoutParams(params);
            square.setGravity(Gravity.CENTER_VERTICAL | Gravity.CENTER_HORIZONTAL);
            square.setTextSize(TypedValue.COMPLEX_UNIT_SP, 35);
            background.addView(square);
            squareList.add(square);
            x += sqWidth + border;
            if((i + 1) % 4 == 0) {
                x = border;
                y += sqWidth + border;
            }
        }

        //adds letters to 4x4 grid
        for(int i = 0; i<16; i++) {
            squareList.get(i).setText(boardString.substring(i, i+1));
        }

        //loads 8 words from solution string into a list
        solutionList = getWords(solutionString);

        //hides menus if touch outside menus
        Screen.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View v, MotionEvent event) {
                if(event.getAction() == MotionEvent.ACTION_DOWN) {
                    MenuView.setVisibility(View.INVISIBLE);
                    MultiMenuView.setVisibility(View.INVISIBLE);
                }
                return false;
            }
        });

        //main touch listener for app
        //switches letters when user drags across 4x4 grid
        //two fingers will switch whole rows/columns
        //three main components: ACTION_DOWN, ACTION_MOVE, and ACTION_UP
        background.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, final MotionEvent motionEvent) {
                if(motionEvent.getActionIndex() >= 1 && !twoFingers) {
                    twoFingers = true;
                    if(Math.abs(motionEvent.getX(motionEvent.getPointerId(1)) - motionEvent.getX()) > Math.abs(motionEvent.getY(motionEvent.getPointerId(1)) - motionEvent.getY())) isVertical = false;
                    else isVertical = true;
                }

                if (motionEvent.getAction() == MotionEvent.ACTION_DOWN) {
                    MenuView.setVisibility(View.INVISIBLE);
                    MultiMenuView.setVisibility(View.INVISIBLE);
                    for (int i = 0; i < squareList.size() && !boardString.equals(solutionString); i++) {
                        if (motionEvent.getX() > squareList.get(i).getLeft() &&
                                motionEvent.getX() < squareList.get(i).getRight() &&
                                motionEvent.getY() > squareList.get(i).getTop() &&
                                motionEvent.getY() < squareList.get(i).getBottom()) {
                            firstPressed = i;
                            if(!twoFingers){
                                squareList.get(i).setBackground(darkGD);
                                firstList.add (i);
                            }
                            else if(isVertical) {
                                for(int ii = 0; ii<squareList.size(); ii++) {
                                    if(ii % 4 == i % 4) {
                                        firstList.add(ii);
                                        squareList.get(ii).setBackground(darkGD);
                                    }
                                }
                            }
                            else {
                                for(int ii = 0; ii<squareList.size(); ii++) {
                                    if(ii/4 == i/4) {
                                        firstList.add(ii);
                                        squareList.get(ii).setBackground(darkGD);
                                    }
                                }
                            }
                            break;
                        }
                    }
                }
                else if (motionEvent.getAction() == MotionEvent.ACTION_MOVE) {
                    if (firstPressed != -1) {
                        for(int i = 0 ; i<squareList.size(); i++) if(!firstList.contains(i)) squareList.get(i).setBackground(lightGD);

                        for (int i = 0; i < squareList.size(); i++) {
                            if (motionEvent.getX() > squareList.get(i).getLeft() &&
                                    motionEvent.getX() < squareList.get(i).getRight() &&
                                    motionEvent.getY() > squareList.get(i).getTop() &&
                                    motionEvent.getY() < squareList.get(i).getBottom()) {
                                if(!twoFingers) {
                                    squareList.get(i).setBackground(darkGD);
                                    break;
                                }
                                else if(isVertical) {
                                    if(firstList.size() == 1) {
                                        firstList.clear();
                                        firstPressed = i;
                                    }
                                    for(int ii = 0; ii<squareList.size(); ii++) {
                                        if(ii % 4 == i % 4) {
                                            squareList.get(ii).setBackground(darkGD);
                                            if(firstList.size() < 4 && !firstList.contains(ii)) firstList.add(ii);
                                        }
                                    }
                                }
                                else {
                                    if(firstList.size() == 1) {
                                        firstList.clear();
                                        firstPressed = i;
                                    }
                                    for(int ii = 0; ii<squareList.size(); ii++) {
                                        if(ii/4 == i/4) {
                                            squareList.get(ii).setBackground(darkGD);
                                            if(firstList.size() < 4 && !firstList.contains(ii)) firstList.add(ii);
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                else if (motionEvent.getAction() == MotionEvent.ACTION_UP) {
                    if (firstPressed != -1) {

                        for (int i = 0; i < squareList.size(); i++) {
                            if(keepSettingBlue)squareList.get(i).setBackground(lightGD);
                            if (motionEvent.getX() > squareList.get(i).getLeft() &&
                                    motionEvent.getX() < squareList.get(i).getRight() &&
                                    motionEvent.getY() > squareList.get(i).getTop() &&
                                    motionEvent.getY() < squareList.get(i).getBottom()) {
                                setBoard(i);
                            }
                        }
                        keepSettingBlue = true;
                        firstPressed = -1;
                        twoFingers = false;
                        firstList.clear();
                    }
                }
                return true;
            }
        });
    }


    //called when user wants a new puzzle
    private void resetPuzzle() {
        QuitButton.setVisibility(View.INVISIBLE);
        isMultiplayerMode = false;
        currId = "default";
        startTime = new Date();
        TimerTextView.setVisibility(View.INVISIBLE);
        setBoardBlue();
        time = 0;
        Random rand = new Random();
        int r = rand.nextInt(333880);
        String text = wordString.substring(r);
        text = text.substring(text.indexOf("\r\n") + 2);
        solutionString = text.substring(0, text.indexOf("\r\n"));
        boardString = scrambleSolution(solutionString);
        for(int i = 0; i<16; i++) {
            squareList.get(i).setText(boardString.substring(i, i+1));
        }
        solutionList = getWords(solutionString);
        guessedList.clear();
    }

    //occurs when user clicks scramble button from menu or new puzzle is loaded
    public String scrambleSolution(String solution) {
        Random rand = new Random();
        ArrayList<Integer> list = new ArrayList<>();
        String temp = "";
        for(int i = 0; i<16; i++) {
            int index = rand.nextInt(16);
            while(list.contains(index)) {
                index++;
                if(index > 15) index = 0;
            }
            temp += solution.substring(index, index + 1);
            list.add(index);
        }
        return temp;
    }

    private void setBoard(int last) {

        //user only has one finger on 4x4 grid
        if(!twoFingers){
            String temp = "";
            for(int i = 0; i<16; i++) {
                if(i == firstPressed) temp += boardString.substring(last, last + 1);
                else if(i == last) temp += boardString.substring(firstPressed, firstPressed + 1);
                else temp += boardString.substring(i, i + 1);
            }
            boardString = temp;
        }
        //user has two fingers placed in a vertical fashion
        //will swap columns
        else if(isVertical) {
            String temp = "";
            for(int i = 0; i<16; i++) {
                if(i%4 == firstPressed%4) temp += boardString.substring((last%4) + ((i/4) * 4), (last%4) + ((i/4) * 4) + 1);
                else if(i%4 == last%4) temp += boardString.substring((firstPressed%4) + ((i/4) * 4), (firstPressed%4) + ((i/4) * 4) + 1);
                else temp += boardString.substring(i, i + 1);
            }
            boardString = temp;
        }
        //user has two fingers placed in horizontal fashion
        //will swap rows
        else {
            String temp = "";
            for(int i = 0; i<16; i++) {
                if(i/4 == firstPressed/4) temp += boardString.substring(((last/4) * 4) + (i%4), ((last/4)*4) + (i%4) + 1);
                else if(i/4 == last/4) temp += boardString.substring(((firstPressed/4)*4) + (i%4), ((firstPressed/4)*4) + (i%4) + 1);
                else temp += boardString.substring(i, i + 1);
            }
            boardString = temp;
        }
        //updates UI
        for(int i = 0; i<16; i++) {
            squareList.get(i).setText(boardString.substring(i, i+1));
        }

        //user has solved puzzle
        if(boardString.equals(solutionString)) {
            keepSettingBlue = false;
            setBoardYellow();
            addTime();
            TimerTextView.setVisibility(View.VISIBLE);
            TimerTextView.setText(formatTime(time));
        }
        //puzzle continues
        else {
            setBoardBlue();
            //will flash yellow on words correctly and recently placed
            checkBoard(last);
        }
    }

    //sets background of all squares in 4x4 grid to light blue gradient
    private void setBoardBlue() {
        background.setBackgroundColor(myBlue);
        for(int i = 0; i<squareList.size(); i++) squareList.get(i).setBackground(lightGD);
    }

    //sets background of all squares in 4x4 grid to yellow gradient
    //called when puzzle is solved
    private void setBoardYellow() {
        background.setBackgroundColor(finalYellow);
        for(int i = 0; i<squareList.size(); i++) squareList.get(i).setBackground(yellowGD);
    }

    //checks if any words in the last square touched are correctly placed
    private void checkBoard(int last) {
        ArrayList<String> currList = getWords(boardString);
        int closeCount = 0;
        for(int i = 0; i<currList.size(); i++) {
            if(currList.get(i).equals(solutionList.get(i)) && (i == last/4 || i-4 == last%4)) {
                //flashes yellow
                animateCorrect(i);
                if(!guessedList.contains(currList.get(i))) guessedList.add(currList.get(i));
            }
            else if(solutionList.contains(currList.get(i)) && !guessedList.contains(currList.get(i))) {
                closeCount++;
                guessedList.add(currList.get(i));
            }
        }
        //flashes green, means it is a correct word but is out of place
        animateClose(closeCount);
    }

    //flashes a row or column yellow to show that the word is in the correct place
    private void animateCorrect(final int position) {
        ValueAnimator colorAnimation = ValueAnimator.ofObject(new ArgbEvaluator(), Color.TRANSPARENT, myYellow);
        colorAnimation.setDuration(700);
        colorAnimation.addUpdateListener(new ValueAnimator.AnimatorUpdateListener() {
            @Override
            public void onAnimationUpdate(ValueAnimator animator) {
                goldList.get(position).setBackgroundColor((int) animator.getAnimatedValue());
            }
        });
        colorAnimation.start();

        ValueAnimator colorAnimation2 = ValueAnimator.ofObject(new ArgbEvaluator(), myYellow, Color.TRANSPARENT);
        colorAnimation2.setDuration(700);
        colorAnimation2.setStartDelay(700);
        colorAnimation2.addUpdateListener(new ValueAnimator.AnimatorUpdateListener() {
            @Override
            public void onAnimationUpdate(ValueAnimator animator) {
                goldList.get(position).setBackgroundColor((int) animator.getAnimatedValue());
            }
        });
        colorAnimation2.start();
    }

    //flashes a row or column green to show that the word is correct but out of place
    private void animateClose(int number) {
        for(int i = 0; i<number; i++) {
            ValueAnimator colorAnimation = ValueAnimator.ofObject(new ArgbEvaluator(), myBlue, myGreen);
            colorAnimation.setDuration(700);
            colorAnimation.setStartDelay(i * 700);
            colorAnimation.addUpdateListener(new ValueAnimator.AnimatorUpdateListener() {
                @Override
                public void onAnimationUpdate(ValueAnimator animator) {
                    background.setBackgroundColor((int) animator.getAnimatedValue());
                }
            });
            colorAnimation.start();

            ValueAnimator colorAnimation2 = ValueAnimator.ofObject(new ArgbEvaluator(), myGreen, myBlue);
            colorAnimation2.setDuration(700);
            colorAnimation2.setStartDelay((i*700) + 700);
            colorAnimation2.addUpdateListener(new ValueAnimator.AnimatorUpdateListener() {
                @Override
                public void onAnimationUpdate(ValueAnimator animator) {
                    background.setBackgroundColor((int) animator.getAnimatedValue());
                }
            });
            colorAnimation2.start();
        }

    }

    //loads eight words of puzzle from solution string of length 16
    //will be 4 rows and 4 columns of 4x4 grid
    private ArrayList<String> getWords(String text) {
        ArrayList<String> l = new ArrayList<>();
        l.add(text.substring(0,4));
        l.add(text.substring(4,8));
        l.add(text.substring(8,12));
        l.add(text.substring(12,16));
        l.add(text.substring(0,1) + text.substring(4,5) + text.substring(8,9) + text.substring(12,13));
        l.add(text.substring(1,2) + text.substring(5,6) + text.substring(9,10) + text.substring(13,14));
        l.add(text.substring(2,3) + text.substring(6,7) + text.substring(10,11) + text.substring(14,15));
        l.add(text.substring(3,4) + text.substring(7,8) + text.substring(11,12) + text.substring(15,16));
        return l;
    }




    /* &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& */
    /* &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& */
    /* &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& */
    /* &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& */
    /* &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& */
    /* &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& */
    /* &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& */
    /* &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& */
    /* &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& */
    /* &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& */
    /* &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& */
    /* &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& */
    /* &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& */
    /* &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&& */


    //brings up alert dialog with provided title and message
    public void showWarning(String title, String message) {
        AlertDialog.Builder alertDialogBuilder = new AlertDialog.Builder(this);
        alertDialogBuilder.setTitle(title).setMessage(message);
        alertDialogBuilder.setCancelable(false).setPositiveButton("OK",
                new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog, int id) {
                        // if this button is clicked, close
                        // current activity
                    }
                });
        mAlertDialog = alertDialogBuilder.create();
        mAlertDialog.show();
    }

    //given a number of seconds, converts to a string of minutes and seconds
    public String formatTime(Integer t) {
        String sec = String.valueOf(t % 60);
        if(sec.length() == 1) sec = "0" + sec;
        String min = String.valueOf(t / 60);
        return min + ":" + sec;
    }

    //adds time spent solving a puzzle to appropriate saved game
    //mainly used for multiplayer purposes
    private void addTime() {
        Date end = new Date();
        int seconds = (int)((end.getTime() - startTime.getTime()) / 1000);
        time += seconds;
        for(int i = 0; i<savedList.size(); i++) if(currId.equals(savedList.get(i).id)) savedList.get(i).id += seconds;
    }

    //saves important data to shared preferences
    private void saveData() {
        pref.edit().putBoolean("signInClicked", signInClicked).apply();
        for(int i = 0; i<savedList.size(); i++) {
            if(savedList.get(i).id.equals(currId)) {
                savedList.get(i).board = boardString;
                savedList.get(i).solution = solutionString;
            }
        }
        String text = "";
        for(int i = 0; i<savedList.size(); i++) {
            text += savedList.get(i).getSaveData() + "|";
        }
        if(text.length() > 0) text = text.substring(0, text.length() - 1);
        pref.edit().putString("games",text).apply();
        pref.edit().putString("id",currId).apply();

        if(currentGame != null) {
            pref.edit().putString("currentGame",currentGame.getDataString()).apply();
        }
    }

    //loads back from shared preferences
    private void getSavedData() {
        try {
            signInClicked = pref.getBoolean("signInClicked", false);
            currId = pref.getString("id", "default");
            String text = pref.getString("games","");
            if(text.length() > 0 && text.contains("|")) {
                try {
                    String[] stArr = text.split("|");
                    for(int i = 0; i<stArr.length; i++) savedList.add(new SavedGame(stArr[i]));
                    for(int i = 0; i<savedList.size(); i++) {
                        if(savedList.get(i).id.equals(currId)) {
                            boardString = savedList.get(i).board;
                            solutionString = savedList.get(i).solution;
                            time = savedList.get(i).time;
                        }
                    }
                }
                catch(Exception er) {}
            }
            else if(text.length() > 0) {
                savedList.add(new SavedGame(text));
                if(savedList.get(0).id.equals(currId)) {
                    boardString = savedList.get(0).board;
                    solutionString = savedList.get(0).solution;
                    time = savedList.get(0).time;
                }
            }

            if(savedList.size() == 0) {
                Random rand = new Random();
                int r = rand.nextInt(333880);
                String t = wordString.substring(r);
                t = t.substring(t.indexOf("\r\n") + 2);
                t = t.substring(0, t.indexOf("\r\n"));
                String b = scrambleSolution(t);
                savedList.add(new SavedGame("default",b,t));
                boardString = b;
                solutionString = t;
                saveData();
            }
            try {
                String cg = pref.getString("currentGame", "");
                if(!cg.equals("")) currentGame = new CurrentGame(cg);
            }
            catch(Exception er){}
        }
        catch(Exception errr){}
    }

    //used for multiplayer mode
    public class SavedGame {
        public String id;
        public Integer time;
        public String board;
        public String solution;

        public SavedGame(String i, String b, String s) {
            id = i;
            time = 0;
            board = b;
            solution = s;
        }

        public SavedGame(String text) {
            id = text.substring(text.indexOf("<id>") + 4, text.indexOf("</id>"));
            time = Integer.parseInt(text.substring(text.indexOf("<time>") + 6, text.indexOf("</time>")));
            board = text.substring(text.indexOf("<board>") + 7, text.indexOf("</board>"));
            solution = text.substring(text.indexOf("<solution>") + 10, text.indexOf("</solution>"));
        }

        public String getSaveData() {
            return "<id>" + id + "</id><time>" + String.valueOf(time) + "</time><board>" + board + "</board><solution>" + solution + "</solution>";
        }
    }

    //holds current multiplayer game
    public class CurrentGame {
        public String matchId;
        public String[] solutions;
        public String[] p1Times;
        public String[] p2Times;
        public String p1Name;
        public String p2Name;

        public CurrentGame(String text) {
            matchId = text.substring(text.indexOf("<MatchId>") + 9, text.indexOf("</MatchId>"));
            String sol = text.substring(text.indexOf("<SolutionStrings>") + 17, text.indexOf("</Solution"));
            solutions = sol.split("\\|");
            String p1T = text.substring(text.indexOf("<P1Times>") + 9, text.indexOf("</P1Times>"));
            p1Times = p1T.split("\\|");
            String p2T = text.substring(text.indexOf("<P2Times>") + 9, text.indexOf("</P2Times>"));
            p2Times = p2T.split("\\|");
            p1Name = text.substring(text.indexOf("<P1Name>") + 8, text.indexOf("</P1Name>"));
            p2Name = text.substring(text.indexOf("<P2Name>") + 8, text.indexOf("</P2Name>"));
            /*try{
                if(p2Name.equals("unknown") && mGoogleApiClient.isConnected() && !p1Name.equals(Games.Players.getCurrentPlayer(mGoogleApiClient).getName())) {
                    p2Name = Games.Players.getCurrentPlayer(mGoogleApiClient).getName();
                }
            }
            catch(Exception er){}*/

        }

        public CurrentGame(String id, String name) {
            matchId = id;

            Random rand = new Random();
            int r = rand.nextInt(333880);
            String text = wordString.substring(r);
            text = text.substring(text.indexOf("\r\n") + 2);
            String s1 = text.substring(0, text.indexOf("\r\n"));

            Random rand2 = new Random();
            r = rand2.nextInt(333880);
            text = wordString.substring(r);
            text = text.substring(text.indexOf("\r\n") + 2);
            String s2 = text.substring(0, text.indexOf("\r\n"));

            Random rand3 = new Random();
            r = rand3.nextInt(333880);
            text = wordString.substring(r);
            text = text.substring(text.indexOf("\r\n") + 2);
            String s3 = text.substring(0, text.indexOf("\r\n"));

            solutions = new String[] {s1, s2, s3};
            p1Times = new String[] {"0","0","0"};
            p2Times = new String[] {"0","0","0"};
            p1Name = name;
            p2Name = "unknown";
        }

        public String getDataString() {
            String text = "<MatchId>" + matchId + "</MatchId>";
            text += "<SolutionStrings>" + solutions[0] + "|" + solutions[1] + "|" + solutions[2] + "</SolutionStrings>";
            text += "<P1Times>" + p1Times[0] + "|" + p1Times[1] + "|" + p1Times[2] + "</P1Times>";
            text += "<P2Times>" + p2Times[0] + "|" + p2Times[1] + "|" + p2Times[2] + "</P2Times>";
            text += "<P1Name>" + p1Name + "</P1Name>";
            text += "<P2Name>" + p2Name + "</P2Name>";
            return text;
        }

        public Integer getRound() {
            Integer round = 2;
            if((p2Times[0]).equals("0")) round = 0;
            else if(p2Times[1].equals("0")) round = 1;
            return round;
        }

        public Integer checkWin() {
            int win1 = 0, win2 = 0;
            if(Integer.parseInt(p1Times[0]) < Integer.parseInt(p2Times[0]) && !p2Times[0].equals("0")) win1++;
            if(Integer.parseInt(p1Times[1]) < Integer.parseInt(p2Times[1]) && !p2Times[1].equals("0")) win1++;
            if(Integer.parseInt(p1Times[2]) < Integer.parseInt(p2Times[2]) && !p2Times[2].equals("0")) win1++;
            if(Integer.parseInt(p1Times[0]) > Integer.parseInt(p2Times[0]) && !p2Times[0].equals("0")) win2++;
            if(Integer.parseInt(p1Times[1]) > Integer.parseInt(p2Times[1]) && !p2Times[1].equals("0")) win2++;
            if(Integer.parseInt(p1Times[2]) > Integer.parseInt(p2Times[2]) && !p2Times[2].equals("0")) win2++;

            if(win1 >= 2) return 1;
            else if(win2 >= 2) return 2;
            else return 0;
        }

        public void addToSavedGames() {
            String board = scrambleSolution(solutions[getRound()]);
            savedList.add(new SavedGame(matchId, board, solutions[getRound()]));
        }
    }











}
