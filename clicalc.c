#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

double calculate(char *input);
float stringToFloat(char* string);
bool isNum(char input);
bool isComma(char input);
void strncpyn(char *dest, char *source, int start, int length);
bool isOp(char input);
int getPriority(char input);
void pntrcpy(char **dest, char **source, int start, int length);
bool isCommand(char *input);
int cmdparser(char *command, char **commands);
double factorial(double input);
bool isVar(char input);
void replaceVar(char *input, int varpos, double value);


//commands
void help(char **commands);
void history(char **inputhistory, double *resulthistory);
void comma();
int mode(int mode);

const int MAXCOMMANDS = 5; //set the amount of commands here 

int main(int argc, char **argv)
{
    char *input = malloc(100);
    if(!input)
    {
        printf("MEMORY ALLOCATION FAILED");
        exit(1);
    }

    char *commands[MAXCOMMANDS] = 
    {
        "help",
        "hist", //1
        "exit", //2
        "comma", //3
        "mode",//4
    };
    char **inputhistory = malloc(500);
    double *resulthistory = malloc(5 * 64);

    int currentmode = 0; //default mode 
    /*mode 0 = simple calculate mode  - works 
     1 = function mode  - not available rn 
     2 = bit mode - not available rn 
     3 = truth mode - not available rn  
     4 = solve mode - not available rn 
     5 = calculus mode - not available rn
    */


    bool running = true;
    while(running)
    {

        printf("Input calculation/function/statement/equation (Type ':' for commands):\n");
        fgets(input, 100, stdin);
        
        if(strlen(input) == 1 && input[0] == '\n')
        {
            continue;
        }
        
        else
        {
            input[strlen(input) - 1] = '\0'; //removes newline 
            
            if(isCommand(input))
            {
                //parse arguments 
                int command = cmdparser(input, commands);
                switch(command)
                {
                    case 0:
                    help(commands);
                    break;

                    case 1: 
                    history(inputhistory, resulthistory);
                    break;

                    case 2:
                    running = false;
                    break;

                    case 3:
                    comma();
                    break;

                    case 4:
                    currentmode = mode(currentmode);
                    break;
                }
            }
            else
            {
                switch(currentmode)
                {
                    case 0:
                    //default mode
                    input[strlen(input)] = ' ';
                    if(isNum(input[0]) || isOp(input[0]) || input[0] == '(' || input[0] == '#')
                    {
                        double result = calculate(input);
                        printf("solution: \n %f\n", result);
                    
                        // Shift history entries to make space for the new entry
                        for (int i = 4; i > 0; i--)
                        {
                            inputhistory[i] = inputhistory[i - 1];
                            resulthistory[i] = resulthistory[i - 1];
                        }

                            // Store the new input and result
                        inputhistory[0] = strdup(input); // Duplicate the input string
                        resulthistory[0] = result;

                    }
                    else
                    {
                        printf("Invalid input !!\n");
                    }
                    break;

                    case 1:
                    //function mode
                    input[strlen(input)] = ' ';
                    if(isNum(input[0]) || isOp(input[0]) || input[0] == '(' || input[0] == '#' || isVar(input[0]))
                    {
                        int start;
                        int interval;
                        int end;
                        printf("inputted function: %s\n", input);
                        printf("Input starting point for variable:\n");
                        scanf("%d", &start);
                        printf("Input interval for variable:\n");
                        scanf("%d", &interval);
                        printf("Input ending point for variable:\n");
                        scanf("%d", &end);
                        //find variable in quation
                        int varpos = -1;
                        int i = 0;
                        while(!isVar(input[i]))
                        {
                            i++;
                        }
                        varpos = i;
                        //test if there is a second variable 
                        i++;
                        for(;i < strlen(input); i++)
                        {
                            if(isVar(input[i]))
                            {
                                printf("SECOND VARIABLE SPOTTED\nCURRENTLY NOT SUPPORTED!\n");
                                exit(1);
                            }
                        }
                        //calculating the values of the functionm
                        char *buffer = malloc(100);
                        strcpy(buffer, input);
                        for(double k = start; k <=  end; k += interval)
                        {
                            replaceVar(buffer, varpos, k);
                            printf("For variable = %f: \t%f", k, calculate(buffer));
                        }
                        replaceVar(buffer, varpos, end);
                        double lastvalue = calculate(buffer);
                        // Shift history entries to make space for the new entry
                        for (int i = 4; i > 0; i--)
                        {
                            inputhistory[i] = inputhistory[i - 1];
                            resulthistory[i] = resulthistory[i - 1];
                        }

                         // Store the new input and result
                        inputhistory[0] = strdup(input); // Duplicate the input string
                        resulthistory[0] = lastvalue;
                        
                    }
                    else
                    {
                        printf("Invalid input !!\n");
                    }
                    break;
                }
                
            }
        }
                
    }

    for(int i = 0; i < 5; i++)
    {
        if(inputhistory[i])
        {
            free(inputhistory[i]);
        }
    }

    free(input);
    free(resulthistory);
    return 0;
}

double calculate(char *input)
{
    
    double solution = 0;
    double *numbers = malloc(strlen(input) * sizeof(double));
    char *operands =  malloc(strlen(input));
    char *orderedops = malloc(strlen(input));
    if(!numbers || !operands || !orderedops)
    {
        printf("Memory allocation failed!!");
        printf("Terminating program...");
        exit(1);
    }

    int neg = 0;
    if(input[0] == '-')
    {
        //first number is negative; store that information and format the string 
        neg = 1;
        for(int i = 0; i < strlen(input); i++)
        {
            input[i] = input[i + 1];
        }
    }
    //fills the array with numbers and operands
    int position = 0;
    int numlength = 0;
    int startnum = -1;

    for(int i = 0; i <= strlen(input); i++)
    {
        //printf("\nposition\t%d\n", position);
        if(isNum(input[i]) || isComma(input[i]))
        {
            //printf("isnummmmm\n");
            if(startnum == -1)
            {
                startnum = i;
            }
            numlength++;
        }
        else
        {
            if(input[i] == ' ')
            {
                continue;
            }
            if(startnum != -1)
            {
                //convert number 
                //printf("numberrrrr convert\n");
                //printf("startnum: %d, numlength %d\n", startnum, numlength);
                char *buffer = malloc(100);
                strncpyn(buffer, input, startnum, numlength);
                //printf("%s\n", buffer);
                numbers[position] = (double) stringToFloat(buffer);
                //printf("%f\n", numbers[position]); 
                operands[position] = '0';
                numlength = 0;
                startnum = -1;
                position++;
                free(buffer);
            }
    
            if(isOp(input[i]))
            {
                operands[position] = input[i];
                numbers[position] = 0;
                position++;
                continue;
            }
            else if(input[i] == '(')
            {
                char *subcalc = malloc(100);
                int j = i + 1; // Start after the opening parentheses
                int parencnt = 1; 
                int subcalcpos = 0; 
                while(parencnt > 0 && j < strlen(input))
                {
                    if(input[j] == '(')
                    {
                        parencnt++;
                    }
                    if(input[j] == ')')
                    {
                        parencnt--;
                    }
                    if(parencnt > 0)
                    {
                        subcalc[subcalcpos] = input[j];
                        subcalcpos++;
                    }
                    
                    j++;
                }
                if((j - i - 1) == 1 || (j - i - 1) == 2)
                {
                    printf("INVALID USE OF PARENTHESES!\n");
                }
                subcalc[subcalcpos] = '\0';
                //evaluate with recurrrsion
                numbers[position] = calculate(subcalc);
                operands[position] = '0';
                position++;

                free(subcalc);
                i = j - 1;
            }
            else if(input[i] == '!')
            {
                numbers[position - 1] = factorial(numbers[position - 1]);
            }
            else if(input[i] == '#')
            {
                int j = i + 1;
                char *tmp = malloc(100);
                int abspos = 0;
                while(input[j] != '#')
                {
                    tmp[abspos] = input[j];
                    abspos++;
                    j++;
                }
                tmp[abspos] = '\0'; //null terminate the string 
                //calculate te value inside the # #
                double value = calculate(tmp); //recursively calculate the value inside the #
                numbers[position] = fabs(value);                    
                operands[position] = '0';
                position++;
                i = j;
                printf("value at 0 %f\n", numbers[0]);
                printf("tmp: %s", tmp);
                free(tmp);  
            }
            else if(input[i] != '\0')
            {
                printf("Wrong character at %d!!\n", i);
            }
            
        }   
    }

    position++;
    operands[position] = '0';

    
    //Debug
    /*printf("DEBUG: ARRAYS:\n");
    printf("Numberws:\n");
    for(int i = 0; i < 5; i++)
    {
        printf("%f\t", numbers[i]);
    }
    printf("Operands: \n %s", operands); */

    //determines order of operations 
    int opcount = 0; //ammount of ops stored 
    for(int i = 0; i <= position; i++)
    {
        if(operands[i] == 0)
        {
            continue;
        }
        int tmp = position;
        char current = operands[i];
        while(tmp > 0 && getPriority(current) > getPriority(orderedops[tmp - 1]))
        {
            tmp--;
        }
        for(int j = opcount; j > tmp; j--)
        {
            orderedops[j] = orderedops[j - 1];   
        }
        orderedops[tmp] = current;
        opcount++;
    }
    //printf("DEBUG: CURRENT ORDER OF OPERATION: \n%s\n", orderedops);


    //remember that check if a number is negative? me neither but now its time to use it 
    if(neg == 1)
    {
        numbers[0] *= -1;
        neg = 0;
    }

    //perform calculation
    
    for(int i = 0; i < opcount; i++)
    {
        /*printf("DEBUG: CURRENT NUMBER ARRAY:\n");
        for(int j = 0; j < opcount; j++)
        {
            printf("%f\n", numbers[j]);
        }*/

        char currentOp = orderedops[i];
        for(int j = 0; j < position; j++)
        {
    
            if(currentOp == operands[j])
            {
                //printf("op found\n");
                switch(currentOp)
                {
                    //perform calculations 
                    case '+' :
                    numbers[j - 1] = numbers [j - 1] + numbers[j + 1];
                    break;

                    case '-' :
                    numbers[j - 1] = numbers [j - 1] - numbers[j + 1];
                    break;

                    case '*' :
                    numbers[j - 1] = numbers [j - 1] * numbers[j + 1];
                    break;

                    case '/' :
                    if(numbers[j + 1] == 0)
                    { 
                        printf("DIVISION BY 0\n");
                        exit(1);
                    }
                    numbers[j - 1] = numbers [j - 1] / numbers[j + 1];
                    break;

                    case '%' :
                    numbers[j - 1] = fmod(numbers[j - 1], numbers[j + 1]);
                    break;

                    case '^' :
                    numbers[j - 1] = (double) pow(numbers[j - 1], numbers[j + 1]);
                    break;
                }
                /*printf("operateddd\n");
                
                printf("Numbers before shifting:\n");
                for(int k = 0; k < position; k++)
                {
                    printf(" %f ", numbers[k]);
                }*/
                //shift the remaining numbers 
                for(int k = j+1; k < position; k+= 2)
                {
                    numbers[k] = numbers[k + 2];
                }
                //shift remaining operands 
                for(int k = j; k < position; k+=2)
                {
                    operands[k] = operands[k + 2];
                }
                //printf("Numbers after shifting:\n");
                //for(int k = 0; k < position; k++)
                //{
                  //  printf(" %f ", numbers[k]);
                //}
            }
            
            
        }
        
    }
        
    solution = numbers[0];

    //freeeeee
    free(numbers);
    free(operands);
    free(orderedops);
    

    return solution;
}


float stringToFloat(char* string)
{
    //removes the \n character if present, otherwise it will mess witht the length
    if(string[strlen(string) - 1] == '\n')
    {
        string[strlen(string) - 1] = '\0';
    }
    int stringlen = strlen(string);

    //finds length of number before comma and indexes the comma to ignore it later
    int precomct = 0;
    bool precom = true;
    int aftcomct = 0;
    int comloc = -1;

    for(int i = 0; i < stringlen; i++)
    {
        if(isNum(string[i]) && precom)
        {
            precomct++;
        }
        else if(isNum(string[i]))
        {
            aftcomct++;
        }
        else
        {
            precom = false;
            comloc = i;
        }
    }

    precomct--; //because on pos 1 you have to have ten to power of 0 
    
    //turn into float
    float sum;
    for(int i = 0; i < stringlen; i++)
    {
        if(isComma(string[i]))
        {

        }
        else
        {
            switch(string[i])
            {
                case '1' :
                sum += pow(10, precomct); 
                break;
                case '2' :
                sum += 2 * pow(10, precomct); 
                break;
                case '3' :
                sum += 3 * pow(10, precomct); 
                break;
                case '4' :
                sum += 4 * pow(10, precomct); 
                break;
                case '5' :
                sum += 5 * pow(10, precomct); 
                break;
                case '6' :
                sum += 6 * pow(10, precomct); 
                break;
                case '7' :
                sum += 7 * pow(10, precomct); 
                break;
                case '8' :
                sum += 8 * pow(10, precomct); 
                break;
                case '9' :
                sum += 9 * pow(10, precomct); 
                break;
                default:
                break;
            }
            precomct--;
        }
    }

    return sum;
}





bool isNum(char input)
{
    char nums[] = {'1', '2', '3', '4', '5', '6', '7' ,'8' ,'9', '0'};
    for(int i = 0; i < 10; i++)
    {
        if(input == nums[i])
        {
            return true;
        }
    }
    return false;
}

bool isComma(char input)
{
    if(input == ',' || input == '.')
    {
        return true;
    }
    return false;
}

void strncpyn(char *dest, char *source, int start, int length)
{
    char *srcP = source + start;
    for(int i = 0; i < length ; i++)
    {
        dest[i] = srcP[i];
    }
    dest[length] = '\0';
}
void pntrcpy(char **dest, char **source, int start, int length)
{
    char **srcP = source + start;
    for(int i = 0; i < length; i++)
    {
        *dest[i] = * srcP[i];
    }

}

bool isOp(char input)
{
    char ops[] = {'+', '-', '*', '/', '^', '%'};   
    int opsLength = sizeof(ops) / sizeof(ops[0]); // Calculate the length of the ops array

    for(int i = 0; i < opsLength; i++)
    {
        if(input == ops[i])
        {
            return true;
        }
    }
    return false;
}

int getPriority(char input)
{
    switch(input)
    {
        case '+' :
        return 0;
        break;
        
        case '-' :
        return 0;
        break;

        case '*' :
        return 1;
        break;

        case '/' :
        return 1;
        break;

        case '^' :
        return 2;
        break;

        case '!' :
        return 3;
        break;

        case '%' :
        return 1;
        break;

        default:
        return -1;
        break;

    }
    return -1;
}

bool isCommand(char *input)
{
    if(input[0] == ':')
    {
        return true;
    }
    return false;
}

int cmdparser(char *command, char **commands)
{
    int cmd = -1;
    if(!command)
    {
        printf("command is null!\n");
    }

    char *cmdStr = command + 1;

    for(int i = 0; i < MAXCOMMANDS;i ++)
    {

        if(strcmp(cmdStr, commands[i]) == 0)
        {
            cmd = i;
            break;
        }
    }
    return cmd;
}

void help(char **commands)
{
    printf("Available commands:\n");
    for(int i = 0; i < MAXCOMMANDS; i++)
    {
        printf("%s\n",commands[i]);
    }
}
void history(char **inputhistory, double *resulthistory)
{
    printf("Calculator history:\n");
    for(int i = 4; i >= 0; i--)
    {
        printf("%s\t\t%f\n", inputhistory[i], resulthistory[i]);
    }
}

void comma()
{
    printf("comma\n");
}

int mode(int mode)
{
    printf("Current mode is:\t");
    switch(mode)
    {
        case 0:
        printf("default \n");
        break;

        case 1:
        printf("function \n");
        break;

        case 2: 
        printf("bitwise \n");
        break;

        case 3: 
        printf("truth \n");
        break;

        case 4: 
        printf("solve \n");
        break;

        case 5:
        printf("calculus \n");
        break;

        default:
        printf("ERROR: NO MODE SELECTED \n");
    }
    printf("Available mode:\n");
    printf("0 - default \t 1 - function \t 2 - bitwise \n3 - truth \t 4 - solve \t 5 - calculus\n");
    printf("Choose mode to switch to:\n");
    int selected;
    scanf("%d", &selected);
    printf("Mode %d selected!\n", selected);
    return selected;
}

double factorial(double input)
{
    double fact = 1;
    for(int i = 1; i <= input; i++)
    {
        fact *= i;
    }
    return fact;
}

bool isVar(char input)
{
    char vars[] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p','q','r','s','t','u','v','w','x','y','z'};   
    int varLength = sizeof(vars) / sizeof(vars[0]); // Calculate the length of the ops array

    for(int i = 0; i < varLength; i++)
    {
        if(input == vars[i])
        {
            return true;
        }
    }
    return false;
}

void replaceVar(char *input, int varpos, double value)
{
    
    char *buffer = malloc(100);
    for(int i = 0; i < varpos; i++) //copies the string till the variable 
    {
        buffer[i] = input[i];
    }
    
    //inserts variable
    char *tmp = malloc(50);
    sprintf(tmp, "%f", value);
    int k = 0;
    int i = varpos;
    while(k < strlen(tmp))
    {
        buffer[i] = tmp[k];
        k++;
        i++;
    }
    
    //insert rest of string 
    int j = varpos + strlen(tmp); // after the inserted variable 
    varpos ++;
    while(input[varpos] != '\0')
    {
        buffer[j] = input[varpos];
        varpos++;
        j++;
    }
    
    strcpy(input, buffer);
    free(buffer);
    free(tmp);

}

