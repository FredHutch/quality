#!/usr/bin/python3

from flask import Flask, render_template, request
import tempfile
import subprocess

app = Flask(__name__)

# Function to process the input
def process_input(dilutions):
    sorted_dilutions = sorted(dilutions, key=lambda x:x[0], reverse=True)    
    with tempfile.NamedTemporaryFile(mode='w+t') as f:
        for i in range(3):
            values = []
            for j in range(len(sorted_dilutions)):
                values.append(str(sorted_dilutions[j][i]))
            string = "\t".join(values)
            string += "\n"
            f.write(string)
        f.seek(0)            
        result = subprocess.run(["/app/src/quality", f.name], capture_output=True, text=True)    
    return result.stdout

@app.route('/', methods=['GET', 'POST'])
def index():
    result = None
    number_sets = []
    if request.method == 'POST':
        # Get the values from the form        
        numbers = request.form.getlist('number')
        for i in range(0, len(numbers), 3):
            number_set = [
                numbers[i],
                numbers[i+1],
                numbers[i+2]
            ]
            number_sets.append(number_set)

        # If any value is empty, set it to '0'
        number_sets = [[value if value else '' for value in number_set] for number_set in number_sets]
        
        # Call the function to process the input
        result = process_input(number_sets)
    
    # Render the template and pass the result if available
    return render_template('index.html', result=result, number_sets=number_sets)

if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0')
    #app.run(debug=True)
