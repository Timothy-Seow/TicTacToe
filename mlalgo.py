import pandas as pd
from sklearn.naive_bayes import CategoricalNB
from sklearn.preprocessing import OrdinalEncoder

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
    # ensure board is a flat list of length 9
    if isinstance(board, list):             # checks if the board is of type list
        flat = []
        for item in board:                  # loop through each move on the given board
            if isinstance(item, list): 
                flat.extend(item)           # adds each of the elements to the flat array
            else:
                flat.append(item)           # adds the element to the back of the flat array
        board = flat

    # normalize length and values
    board = [str(x).upper() if x is not None else "B" for x in board]   # list comprehension to convert all to string and uppercase it, then 'B' if doesn't exist
    
    if len(board) < 9:                      # to make sure the board is of length 9    
        board += ["B"] * (9 - len(board))   # add 'B' to board if less than 9 
    else:
        board = board[:9]                   # keep only the first 9 elements of the board array

    
    move = None
    best = -1

    for i in range(9):                      # loop through to find 'B' spots in the board
        if board[i] != "B":                 # skip if current spot is not 'B'
            continue

        new_board = board.copy()            # make a copy of the board
        new_board[i] = "O"                  # replace current spot with 'O' to simulate AI move

        new_board_df = pd.DataFrame([new_board], columns=[f"cell{j}" for j in range(9)])    # convert to pandas dataframe using list comprehension to get board as 1 row of data
        new_board_encoded = encoder.transform(new_board_df)     # encode the new board to 0, 1 and 2s

        prob = model.predict_proba(new_board_encoded)[0][list(model.classes_).index("POSITIVE")]    # give a probability of the board being positive outcome based on the current 'X' spot on the board

        if prob > best:                     # if the probability is more than current best
            best = prob                     # replace the current best probability to the new one
            move = i                        # replace move to the current move
    
    return move

def decode_board(board_str):
    rows = [r.strip() for r in board_str.split(";") if r.strip() != ""]         # split based on ';' in the string
    board = []
    for row in rows:                        # loop through each row in rows
        cols = [c.strip().upper() for c in row.split(",") if c.strip() != ""]   # remove empty spaces and uppercase each character after splitting based on ','
        board.extend(cols)                  # add all the elements to the board array
    
    # normalize to exactly 9 cells
    if len(board) < 9:
        board += ["B"] * (9 - len(board))   # add 'B' to the end of the board until it reaches length 9
    else:
        board = board[:9]                   # keep only the first 9 elements of the board array
    return board                            # return the cleaned board

def main(board_str):

    board = decode_board(board_str)         # decode the board given from the C code
    df, y = load_data("tic-tac-toe.data")   # load the data from the given data file

    encoder = OrdinalEncoder(categories=[["B","O","X"]] * 9, dtype=int)         # encoder to force transform B -> 0, O -> 1, X -> 2
    X_encoded = encoder.fit_transform(df)   # convert the dataframe into numbers

    model = CategoricalNB()                 
    model.fit(X_encoded, y)                 # train the model using the encoded data and results from the data file
    return predict(board, model, encoder)   # returns the prediction based on the train model, encoder and the given board