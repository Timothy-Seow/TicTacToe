import pandas as pd
from sklearn.naive_bayes import CategoricalNB
from sklearn.preprocessing import OrdinalEncoder
from sklearn.model_selection import train_test_split
from sklearn.metrics import confusion_matrix
from sklearn.metrics import classification_report
import matplotlib.pyplot as plt
import numpy as np
import random

def load_data(file):
    boards = []
    results = []

    with open(file, "r") as file:
        for line in file:                   # read every line in the data file
            line = line.strip()             # remove empty spaces front and back
            line = line.upper()             # uppercase every character

            if not line:                    # check and skip if empty line
                continue
                
            parts = line.split(",")         # split characters based on comma
            boards.append(parts[:9])        # append to board list the first 9 characters, which is the current board
            results.append(parts[9])        # append to results list the outcome of the board
        
    df = pd.DataFrame(boards, columns=[f"cell{i}" for i in range(9)])   # list comprehension to convert to a pandas dataframe for each board
    y = pd.Series(results)                  # convert results list to a pandas array

    return df, y

def predict(board, model, encoder):
    
    move = None
    best = -1

    for i in range(9):                      # loop through to find 'B' spots in the board
        if board[i] != "B":                 # skip if current spot is not 'B'
            continue

        new_board = board.copy()            # make a copy of the board
        new_board[i] = "O"                  # replace current spot with 'O' to simulate AI move

        new_board_df = pd.DataFrame([new_board], columns=[f"cell{j}" for j in range(9)])    # convert to pandas dataframe using list comprehension to get board as 1 row of data
        new_board_encoded = encoder.transform(new_board_df)     # encode the new board to 0, 1 and 2s

        prob = model.predict_proba(new_board_encoded)[0][list(model.classes_).index("NEGATIVE")]    # give a probability of the board being negative outcome based on the current 'O' spot on the board

        if prob > best:                              # if the probability is more than current best
            best = prob                              # replace the current best probability to the new one
            move = i                                 # replace move to the current move
    
    return move

def decode_board(board_str):
    board = [c.strip().upper() if c.strip() != "" else "B" for c in board_str.split(",")]   # list comprehension to empty spaces and uppercase it after splitting based on ',', then 'B' if empty

    # normalize to exactly 9 cells
    if len(board) < 9:
        board += ["B"] * (9 - len(board))            # add 'B' to the end of the board until it reaches length 9
    else:
        board = board[:9]                            # keep only the first 9 elements of the board array
    return board                                     # return the cleaned board


def evaluate():
    df, y = load_data("tic-tac-toe.data")            # load the data from the given data file
    encoder = OrdinalEncoder(categories=[["B","O","X"]] * 9, dtype=int)         # encoder to force transform B -> 0, O -> 1, X -> 2
    X_encoded = encoder.fit_transform(df)            # convert the dataframe into numbers
    x_train, x_test, y_train, y_test = train_test_split(X_encoded, y, test_size=0.2, random_state=42, shuffle=True) # split the data into training and testing sets
    model = CategoricalNB()                          # create the model
    model.fit(x_train, y_train)                      # train the model using the training data
    predictions = model.predict(x_test)              # make predictions using the testing data

    cm = confusion_matrix(y_test, predictions)       # compute the confusion matrix
    cr = classification_report(y_test, predictions)  # generate a classification report
    
    labels = ["POSITIVE", "NEGATIVE"]                # define the labels for the confusion matrix
    plt.figure(figsize=(6,6))                        # set the figure size
    plt.imshow(cm, interpolation='nearest')          # display the confusion matrix as an image
    plt.title('Confusion Matrix')                    # set the title
    plt.colorbar()                                   # add a color bar   
    tick_marks = np.arange(len(labels))              # set the tick marks
    plt.xticks(tick_marks, labels)                   # set the x-axis ticks
    plt.yticks(tick_marks, labels)                   # set the y-axis ticks
    plt.xlabel('Predicted Label')                    # set the x-axis label
    plt.ylabel('True Label')                         # set the y-axis label
    for i in range(cm.shape[0]):                     # include counts in the confusion matrix
        for j in range(cm.shape[1]):                
            plt.text(j, i, cm[i, j], ha="center", va="center", color="white" if cm[i, j] > cm.max()/2 else "black")
    plt.tight_layout()                               # adjust layout to prevent overlap
    plt.show()                                       # display the plot

    print("Classification Report:")
    print(cr)                                        # print the classification report               

    win_rate(model, encoder)                         # evaluate the win rate of the model

def win_rate(model, encoder):

    results = {"Win":0, "Loss":0, "Draw":0}

    for game in range(100):           # Simulate 100 games
        board = ["B"] * 9             # Start with an empty board
        results[play(board, model, encoder)] += 1
    
    print(f"AI Win Rate: {results['Win']} Wins, {results['Loss']} Losses, {results['Draw']} Draws")    

def play(board, model, encoder):
    # Winning combinations
    wins = [[0,1,2],[3,4,5],[6,7,8],  # Rows
            [0,3,6],[1,4,7],[2,5,8],  # Cols
            [0,4,8],[2,4,6]]          # Diagonals

    while True:
        x_move = random.choice([i for i in range(9) if board[i] == "B"])        # Random move for X
        board[x_move] = "X"
        for win in wins:
            if all(board[i] == "X" for i in win):                               # X wins
                return "Loss"                                                   # AI loses
            if all(board[i] != "B" for i in range(9)):                          # Board is full
                return "Draw"                                                   # Game is a Draw             

        o_move = predict(board.copy(), model, encoder)                          # AI move for O
        board[o_move] = "O"                 
        for win in wins:                    
            if all(board[i] == "O" for i in win):                               # O wins
                return "Win"                                                    # AI wins
            if all(board[i] != "B" for i in range(9)):                          # Board is full
                return "Draw"                                                   # Game is a Draw

def main(board_str):

    board = decode_board(board_str)         # decode the board given from the C code
    df, y = load_data("tic-tac-toe.data")   # load the data from the given data file

    encoder = OrdinalEncoder(categories=[["B","O","X"]] * 9, dtype=int)         # encoder to force transform B -> 0, O -> 1, X -> 2
    X_encoded = encoder.fit_transform(df)   # convert the dataframe into numbers

    model = CategoricalNB()                 
    model.fit(X_encoded, y)                 # train the model using the encoded data and results from the data file
    return predict(board, model, encoder)   # returns the prediction based on the train model, encoder and the given board

# evaluate() # Uncomment to run evaluation of the model