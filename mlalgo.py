import pandas as pd
from sklearn.naive_bayes import CategoricalNB
from sklearn.preprocessing import OrdinalEncoder

def load_data(file):
    boards = []
    labels = []

    with open(file, "r") as file:
        for line in file:
            line = line.strip()
            line = line.upper()

            if not line:
                continue
        
                
            parts = line.split(",")
            boards.append(parts[:9])
            labels.append(parts[9])

        
    df = pd.DataFrame(boards, columns=[f"cell{i}" for i in range(9)])
    y = pd.Series(labels)

    return df, y

df, y = load_data("tic-tac-toe.data")

encoder = OrdinalEncoder() # encoder to transform X -> 2, O -> 1, B -> 0
X_encoded = encoder.fit_transform(df)


model = CategoricalNB()
model.fit(X_encoded, y)

def predict(board, model, encoder):
    move = None
    best = -1

    for i in range(9):
        if board[i] != "B":
            continue

        new_board = board.copy()
        new_board[i] = "X"

        new_board_df = pd.DataFrame([new_board], columns=[f"cell{j}" for j in range(9)])
        new_board_encoded = encoder.transform(new_board_df)

        prob = model.predict_proba(new_board_encoded)[0][list(model.classes_).index("POSITIVE")]

        if prob > best:
            best = prob
            move = i
    
    return move

# current_board = pd.DataFrame([["X", "O", "B", "B", "X", "B", "O", "B", "B"]])
current_board = ["X", "O", "B", "B", "X", "B", "O", "B", "B"]
move = predict(current_board, model, encoder)

print("Current Board:")
for i in range(9):
    print(current_board[i], end=" ")
    if (i + 1) % 3 == 0:
        print()

print("\nPredicted best move index:", move, "(row {}, col {})".format(move//3, move%3))