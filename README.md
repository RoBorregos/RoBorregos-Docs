# RoBorregos-Docs
RoBorregos Official Documentation

Welcome to the RoBorregos Official documentation. This documentation is based on mkdocs, using the material theme. 

### Development team

| Name                    | Email                                                               | Github                                                       | Role      |
| ----------------------- | ------------------------------------------------------------------- | ------------------------------------------------------------ | --------- |
| Iván Romero | [i.wells.ar@gmail.com](mailto:i.wells.ar@gmail.com) | [@IvanRomero03](https://github.com/IvanRomero03) | Repo Mantainer and Documentation Lead |


## Add new page

To add a new page, locate the docs directory.
```{bash}
ROBORREGOS-DOCS
│   mkdocs.yml
│   requirements.txt
│
└───docs
│   │   index.md
│   │   YourNewPage.md
│   └───assets
│           favicon.png
│           logo.png
│           ...
│
└───sites
        ...
```
Here, add a new **.md** file and add your content.
To add new images, add them to the assets folder. Preferably, use the same name as the page you are adding. Example: If you are adding a new page called **YourNewPage.md**, add your images to the assets folder with the name **YourNewPage**.

## Run locally

To run the documentation locally, you need to have python installed.

1. Clone the repository
```{bash}
git clone https://github.com/RoBorregos/RoBorregos-Docs.git
```

2. Install the requirements
```{bash}
pip install -r requirements.txt
```

3. Run the server
```{bash}
mkdocs serve
```

4. Open the browser and go to http://localhost:8000

## Deploy
Pls do not deploy without permission from the repo mantainer.
```{bash}	
mkdocs gh-deploy
```

