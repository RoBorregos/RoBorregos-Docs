# GPT-3 API

We use OpenAI's GPT-3 API to get the entities of the text, to use the api in a fast way we use the playground that OpenAI provides, you can find it [here](https://platform.openai.com/playground/p/overview?mode=complete&model=text-davinci-003). There you can ask for the commands to check if your request is correct, and then you can use the code that they provide to make the request in your dictionary.

Tha base code for an the used parameters is the following:

```python
def __init__(self):
    openai.api_key = os.getenv("OPENAI_API_KEY")
    # GPT API
    self.GPT_temperature=0.7
    self.GPT_top_p=1
    self.GPT_frequency_penalty=0    
    self.GPT_presence_penalty=0
```
<small>*Remember you should put your API Key as a enviroment variable*</small>

And a call should look like this: 

```python
intent = self.callGPT((command + " " + self.inputText))

def callGPT(self, pr, t_max=256):
        response = openai.Completion.create(
            model=self.GPT_model,
            prompt=pr,
            temperature=self.GPT_temperature,
            max_tokens=t_max,
            top_p=1,
            frequency_penalty=self.GPT_frequency_penalty,
            presence_penalty=self.GPT_presence_penalty, 
        )
        return response.choices[0].text
```

<small>*t_max is the maximium of tokens you can use in the call, if the prompt is too long it will return an error, so you can try to raise the t_max value.*</small>

## Entities extraction
When we wnt to extract the entities from a text we use the dictionary of calls that we have previously tested in the playground, some examples of calls are:

```python
calls = {
"get_name": "if there is nos name return ''get me the name of the person on the next message: ",
"get_drink": "get me the name of the beverage on the next message: ",
"describe": "describe a person with the next attributes: ",
"confirm" : "tell me onlyTrue or False, the next message is a general confirmation, like ues, OK, got it:",
"reached": "tell me only True or False, the next message is a confirmation that you reached a place:",

"get_loc": "the valid locations are living room, dining room, kitchen, bedroom. Give me only the location on this message: ",
"get_obj": "the valid objects are coke, apple, mug, soap, banana, pitcher. Now give me only the object on this message: ",
"get_per": "the valid names are Jamie, Morgan, Micheal, Jordam, Taylor, Tracy, Robin, Alex. Now give me only the name on this mesage: "
}
```
You need to be very specific with the calls, because if you are not, the response will not be the expected and be sure that returns only the entity that you want, because as a completition model it will try to complete the text with more information.

## OpenAI Tokens 
The tokens are the way that OpenAI uses to charge the API, you can check the tokens that you have used in the [billing page](https://platform.openai.com/account/billing/overview). The tokens are charged by the number of tokens that you use in the call, remember you have a free trial of 30000 tokens, so you can use it to test the API. 18 dollars and that should be more than enough for testing the API. Be carefull with the expiration date of trial.

### Previous test

Because GPT-3 API is a powerfull language model we tried to give most of the control to GPT-3 API, but we found that it is not the best way to use it, because as a language model we specified a input and an output format but it doesnt give a stable response and dont follow the rules that you gave him, specially when involves state machines and the response ould depend on the previous state. So we decided to give more control to the code and use GPT-3 API only to get the entities of the text.