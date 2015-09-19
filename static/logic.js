var X_SYMBOL = 'x';
var O_SYMBOL = 'o';
/* holds ascii mark of the human player */
var HUMAN_PLAYER;

/* holds ascii mark of the ai player */
var AI;
/* 2d array of jQuery div elements corresponding to board positions*/
var CELLS;
/* 2d array of ascii characters containing the current board layout*/
var BOARD;
var ALLOW_HUMAN_MOVE;

var GAME_STATUS_BOX = $(".game-status");

var SELECTED_CLASS = 'selected-border';
var ELEMENT_X_MARK_CHOICE = $('#x-mark-choice');
var ELEMENT_O_MARK_CHOICE = $('#o-mark-choice');
var ELEMENT_AI_FIRST = $('#ai-first');
var ELEMENT_HUMAN_FIRST = $('#human-first');
var ELEMENT_LEGEND_HM = $('.human-choice');
var ELEMENT_LEGEND_CM = $('.computer-choice');

var START_BUTTON = $('#start-button');
var TRY_AGAIN_BUTTON = $('#try-again-button');

var TIC_TAC_BOARD = $('.tt-container');
var BEGIN_GAME_VIEW = $('.start-selection');
var LEGEND_VIEW = $('.mark-legend');

setupBeginGameViewListeners();
setupGamePlayListeners();

function setupBeginGameViewListeners() {
  /* 
    begins game, essentially an entry point
  */
  START_BUTTON.click(function(event){
    event.preventDefault();
    var sign_picked = ELEMENT_X_MARK_CHOICE.hasClass(SELECTED_CLASS) 
      || ELEMENT_O_MARK_CHOICE.hasClass(SELECTED_CLASS);
    var starting_player_picked = ELEMENT_AI_FIRST.hasClass(SELECTED_CLASS)
      || ELEMENT_HUMAN_FIRST.hasClass(SELECTED_CLASS);
    if(sign_picked && starting_player_picked) {
      initializeData();
      beginGame();
    } else if(sign_picked) {
      alert("You did not pick who should start first!");
    } else if(starting_player_picked) {
      alert("You did not pick your mark!");
    } else {
      alert("Pick your mark and starting player!");
    }
  });

  /*
    Controls the selection of 'x' mark for the user
   */
  ELEMENT_X_MARK_CHOICE.click(function() {
    if(ELEMENT_O_MARK_CHOICE.hasClass(SELECTED_CLASS)) {
      ELEMENT_O_MARK_CHOICE.toggleClass(SELECTED_CLASS);
    }
    $(this).toggleClass(SELECTED_CLASS);
  });

  /*
    Controls the selection of 'o' mark for the user
   */
  ELEMENT_O_MARK_CHOICE.click(function() {
    if(ELEMENT_X_MARK_CHOICE.hasClass(SELECTED_CLASS)) {
      ELEMENT_X_MARK_CHOICE.toggleClass(SELECTED_CLASS);
    }
    $(this).toggleClass(SELECTED_CLASS);
  });

  /*
    Controls the selection of AI going first
   */
  ELEMENT_AI_FIRST.click(function() {
    if(ELEMENT_HUMAN_FIRST.hasClass(SELECTED_CLASS)) {
      ELEMENT_HUMAN_FIRST.toggleClass(SELECTED_CLASS);
    }
    $(this).toggleClass(SELECTED_CLASS);
  });

  /*
    Controls the selection of human going first
   */
  ELEMENT_HUMAN_FIRST.click(function() {
    if(ELEMENT_AI_FIRST.hasClass(SELECTED_CLASS)) {
      ELEMENT_AI_FIRST.toggleClass(SELECTED_CLASS);
    }
    $(this).toggleClass(SELECTED_CLASS);
  });
}

function setupGamePlayListeners() {
  /*
    Sets up board to be clickable
   */
  $('.row-0').delegate('div', 'click', function() {
    index = $(this).index();
    setCell(0, index, HUMAN_PLAYER);
  });
  $('.row-1').delegate('div', 'click', function() {
    index = $(this).index();
    setCell(1, index, HUMAN_PLAYER);
  });
  $('.row-2').delegate('div', 'click', function() {
    index = $(this).index();
    setCell(2, index, HUMAN_PLAYER);
  });

  /*
    Resets the some of the variables as well as the visual board for next iteration
   */
  TRY_AGAIN_BUTTON.click(function(event){
    event.preventDefault();

    resetBeginGameView();

    GAME_STATUS_BOX.hide();
    $(this).hide();
    TIC_TAC_BOARD.hide();
    BEGIN_GAME_VIEW.show();

    resetBoard();
    resetLegend();
  });
}

function initializeData() {
  if(ELEMENT_X_MARK_CHOICE.hasClass(SELECTED_CLASS)) {
    HUMAN_PLAYER = X_SYMBOL;
    AI = O_SYMBOL;
    ELEMENT_LEGEND_HM.addClass(X_SYMBOL);
    ELEMENT_LEGEND_CM.addClass(O_SYMBOL);
  } else if(ELEMENT_O_MARK_CHOICE.hasClass(SELECTED_CLASS)) {
    HUMAN_PLAYER = O_SYMBOL;
    AI = X_SYMBOL;
    ELEMENT_LEGEND_HM.addClass(O_SYMBOL);
    ELEMENT_LEGEND_CM.addClass(X_SYMBOL);
  }
  CELLS = [[],[],[]];
  BOARD = [["-","-","-"],["-","-","-"],["-","-","-"]];
  
  if(ELEMENT_HUMAN_FIRST.hasClass(SELECTED_CLASS)) {
    /*player start*/
    ALLOW_HUMAN_MOVE = true;
  } else if(ELEMENT_HUMAN_FIRST.hasClass(SELECTED_CLASS)) {
    /*ai start*/
    ALLOW_HUMAN_MOVE = false;
  }

  /* fetches cell div jquery elements */
  var c = 0;
  for(var i = 0; i < 3; i++) {
    for(var j = 0; j < 3; j++) {
      class_name = ".cell-" + c;
        CELLS[i][j] = $(class_name);
      c++;
    }
  }
}

function beginGame() {
  if(ALLOW_HUMAN_MOVE) {
    GAME_STATUS_BOX.text("Begin by clicking a box to make your move!");
    GAME_STATUS_BOX.show();
  } else {
    requestAIMove();
  }
  BEGIN_GAME_VIEW.hide();
  TIC_TAC_BOARD.show();
  LEGEND_VIEW.show();
}

/*
  Sets the mark in the in memory board as well as takes care of the visual
  effect of adding x and o to the board that is seen by user
 */
function setCell(x, y, player) {
	if (ALLOW_HUMAN_MOVE && BOARD[x][y] == "-") {
		CELLS[x][y].addClass(player);
		BOARD[x][y] = player;
		if(player == HUMAN_PLAYER) {
			ALLOW_HUMAN_MOVE = false;
			requestAIMove();
		}
	}
}

/*
  Converts current board to one-line string. Used for sending board to 
  backend AI.
 */
function boardToString() {
	var str_board = "";
	for(var i = 0; i < 3; i++) {
		for(var j = 0; j < 3; j++) {
			str_board += BOARD[i][j];
		}
	}
	return str_board;
}

/*
  Fires backend call and calls 'makeAIMove' to execute the provided move.
 */
function requestAIMove() {
	$.ajax({
		type: "GET",
		dataType: "json",
		url: "/get_move",
		data: {
				"board": boardToString(),
				"player": AI
			  },
  			success: function(data) {
          GAME_STATUS_BOX.hide();
  				makeAIMove(data["move_x"], data["move_y"], data["winner"]);
  			},
  			error: function(xhr, textStatus, errorThrown){
  				console.log("failure " + errorThrown);
	      }
	}); 
}

/*
  Makes AI move and determines game condidtion. IE lose/draw because user
  win never occurs.
 */
function makeAIMove(x, y, win) {
	ALLOW_HUMAN_MOVE = true;
  if(x != -1 && y != -1) {
    setCell(x, y, AI);
  }
	if(win == "-") {
		GAME_STATUS_BOX.text("It is a draw!");
		GAME_STATUS_BOX.show();
    ALLOW_HUMAN_MOVE = false;
    TRY_AGAIN_BUTTON.show();
	} else if (win == AI) {
		GAME_STATUS_BOX.text("YOU LOSE!!");
		GAME_STATUS_BOX.show();
    ALLOW_HUMAN_MOVE = false;
    TRY_AGAIN_BUTTON.show();
	}
	// user NEVER wins so no point for else case
}

function resetLegend() {
  ELEMENT_LEGEND_HM.removeClass(O_SYMBOL);
  ELEMENT_LEGEND_HM.removeClass(X_SYMBOL);
  ELEMENT_LEGEND_CM.removeClass(O_SYMBOL);
  ELEMENT_LEGEND_CM.removeClass(X_SYMBOL);
  LEGEND_VIEW.hide();
}

function resetBoard() {
  for(var i = 0; i < 3; i++) {
    for(var j = 0; j < 3; j++) {
      CELLS[i][j].removeClass(O_SYMBOL);
      CELLS[i][j].removeClass(X_SYMBOL);
    }
  }
}

function resetBeginGameView() {
  ELEMENT_X_MARK_CHOICE.removeClass(SELECTED_CLASS);
  ELEMENT_O_MARK_CHOICE.removeClass(SELECTED_CLASS);
  ELEMENT_HUMAN_FIRST.removeClass(SELECTED_CLASS);
  ELEMENT_AI_FIRST.removeClass(SELECTED_CLASS);
}